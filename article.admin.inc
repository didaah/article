<?php
// $Id$

/**
 * 内容管理列表
 */
function article_admin_lists() {
  
  if (!$cat = article_get_cat_cid(arg(3))) {
    dd_get_not();
  }
  
  $tabs = array(
    l(t('article', '管理') . $cat->name, 'admin/article/list/' . $cat->cid),
    l(t('article', '添加') . $cat->name, 'admin/article/add/' . $cat->cid),
  );

  dd_set_tabs($tabs);

  if (!empty($_GET['op']) && $_GET['op'] == 'delete') {
    if (user_access('article admin delete') && !empty($_GET['id'])) {
      if (article_delete($_GET['id'])) {
        echo 'tr';
      }
    } else {
      echo '您没有删除内容的权限';
    }
    exit;
  }
 
  $query = 'SELECT n.title, n.username, n.nid, n.uid, n.visit, n.comment_count, n.status, 
  n.created, u.name FROM {article} n INNER JOIN {users} u ON n.uid = u.uid WHERE n.cid = ?';
  
  $args = array($cat->cid);
  
  if (!empty($_GET['keyword'])) {
    $args[] = '%' . urldecode($_GET['keyword']) . '%';
    $query .= ' AND title LIKE ?';
  }
  
  $output = dd_get_form('form_admin_search');
  
  $header = array(
    array('data' => '标题', 'field' => 'n.nid', 'sort' => 'desc'),
    array('data' => '发布人', 'field' => 'n.uid'),
    '显示作者',
    array('data' => '评论数量', 'field' => 'n.comment_count'),
    array('data' => '访问次数', 'field' => 'n.visit'),
    array('data' => '状态', 'field' => 'n.status'),
    array('data' => '发布时间 ', 'field' => 'n.created'),
    '操作'
  );
  
  if ($fetch = pager_query($query . table_sql($header), 20, 0, NULL, $args)) {
    $delete_url = url('admin/article/list/' . $cat->cid, array('query' => 'op=delete'));
    foreach ($fetch as $o) {
      $table[] = array(
        l($o->title, 'article/' . $o->nid, array('attributes' => array('target' => '_blank'))),
        $o->name,
        $o->username,
        $o->comment_count,
        $o->visit,
        $o->status ? '已发布' : '未发布',
        format_date($o->created),
        dd_imp(array(
          l('编辑', 'admin/article/edit/' . $o->nid, dd_redirect()),
          '<a href="' . $delete_url . '&id=' . $o->nid . '" class="confirmajax">删除</a>'
        ))
      );
    }
    
    $output .= theme('table', $header, $table, array('class' => 'center_table'));
    $output .= pager_view();
    
  } else {
    $output .= system_no_content();
  }
  
  return $output;
}

/**
 * 管理后台内容维护表单
 * @param (object) $article
 */
function article_admin_form($article = NULL) {
  $args = arg();

  if (empty($article)) {

    if (!$cat = article_get_cat_cid()) {
      dd_set_message('请先添加一个产品类别', 'error');
      dd_goto('admin/article/category');
    }

    if (!empty($args[3]) && !empty($cat[$args[3]])) {
      
      $article = new stdClass;
      $article->cid = $args[3];

    } else {

      $item = array();
      
      foreach ($cat as $o) { 
        $item[] = l($o->name, 'admin/article/add/' . $o->cid);
      }

      $output = theme('item_list', $item, '请选择一个内容类别');

      return $output;

    }
  }

  $obj = $cat[$args[3]];
  
  $tabs = array(
    l('管理' . $obj->name, 'admin/article/list/' . $obj->cid),
    l('添加' . $obj->name, 'admin/article/add/' . $obj->cid),
  );
  
  if (!empty($article->nid)) {
    $tabs[] = l('编辑' . $obj->name, $_GET['q']);
  }

  dd_set_tabs($tabs);

  $output = dd_get_form('article_form', $article);

  return $output; 
}
