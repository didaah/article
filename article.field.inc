<?php
// $Id$

/**
 * 内容分类
 */
function article_field_category() {
  $tabs = array(
    l('内容分类', 'admin/article/category'),
    l('添加分类', 'admin/article/category/add'),
  );

  $args = arg();
  
  switch ($args[3]) {
    case 'fields': // 分类属性
       
      $field = new stdClass;

      if (is_numeric($args[4]) && $cat = db_query('SELECT cid, name, type 
      FROM {article_cat} WHERE cid = ?', array($args[4]), array('return' => 'one'))) {
        $field->type = 'ext_article_' . $cat->type;
      } else {
        $cat = new stdClass;
        $cat->name = '通用';
        $field->type = $cat->cid = $cat->type = 'default';     
      }

      if (user_access('article admin fields')) {
        $tabs[] = l($cat->name . '属性管理', 'admin/article/category/fields/' . $cat->cid);
        $tabs[] = l($cat->name . '属性添加', 'admin/article/category/fields/' . $cat->cid . '/add');
        $tabs[] = l($cat->name . '属性导出', 'admin/article/category/fields/' . $cat->cid . '/export');
      }
      
      switch ($args[5]) {
        case 'add':
          $field->module = 'article';
          $field->redirect = url('admin/article/category/fields/' . $cat->cid);
          $output = dd_get_form('field_form', $field);
        break;
        
        case 'export':
          $output = field_export(0, 'article', $field->type);
        break;
        
        default:
          $output = field_lists('article', $field->type);
      }
     
    break;
    
    case 'add': case 'edit': // 添加/编辑内容分类
      
      if (!empty($args[4]) && is_numeric($args[4])) {
        $cat = db_query('SELECT * FROM {article_cat} WHERE cid = ?', array($args[4]), array('return' => 'one'));
      } else {
        $cat = NULL;
      }
      
      $output = dd_get_form('article_field_category_form', $cat);
      
    break;
    
    case 'delete': // 删除分类
      if ($args[4] && is_numeric($args[4])) {
        if (db_query('SELECT COUNT(nid) FROM {article} WHERE cid = ?',
        array($args[4]), array('return' => 'column'))) {
          echo '该分类下有多篇文章，无法删除，请先将产品移至其它分类';
        } else if (db_exec('DELETE FROM {article_cat} WHERE cid = ?', array($args[4]))) {
          // 删除成功
          
          // 删除分类属性
          db_exec('DELETE FROM {fields} WHERE module = ? AND type = ?', array('shop', 'ext_shop_' . $args[4]));
          
          // 更新分类缓存
          cache_del('cid', 'article_get_cat_type');
          cache_del('cid', 'article_get_cat_cid');
          cache_del('*', 'admin_menus_');
          
          // ajax 返回值
          echo 'tr';
        } else {
          echo '删除失败';
        }
      } else {
        echo '不合法的操作';
      }
    exit;
    
    default:
      // 管理扩展属性的权限
      $is_field = user_access('article admin fields');

      $table = array(
        array(
          t('article', '通用属性'),
          t('article', '已启用'), 
          ($is_field ? l(t('article', '属性管理'), 'admin/article/category/fields/default') : ''),
        )
      );

      if ($fetch = db_query('SELECT * FROM {article_cat} ORDER BY pid ASC')) {

        foreach ($fetch as $o) {
          $links = array(
            l('编辑', 'admin/article/category/edit/' . $o->cid),
            l('删除', 'admin/article/category/delete/' . $o->cid, array('attributes' => array('class' => 'confirmajax'))),
          );

          if ($is_field) {
            $links[] = l('属性管理', 'admin/article/category/fields/' . $o->cid);
          }

          $table[] = array(
            l($o->name, 'article/category/' . $o->cid),
            $o->status ? '已启用' : '未启用',
            dd_imp($links)
          );
        }
      }
      
      $output = theme('table', array('分类', '状态', '操作'), $table, array('class' => 'center_table'));
      $output .= pager_view();
  }
  
  dd_set_tabs($tabs);
  
  return $output;
}

/**
 * 产品分类表单
 * @param object $cat
 */
function article_field_category_form($cat = NULL) {
  $form['fields']['name'] = array(
    '#type' => 'textfield',
    '#required' => 1,
    '#default_value' => $cat->name,
    '#title' => '分类名称'
  );
  
  $form['fields']['type'] = array(
    '#type' => 'textfield',
    '#required' => 1,
    '#default_value' => $cat->type,
    '#description' => '只能是数字、字母、下划线，长度 12 个字符内',
    '#title' => '分类索引'
  );
  
  $form['fields']['status'] = array(
    '#type' => 'radio',
    '#title' => '分类状态',
    '#default_value' => $cat->status,
    '#required' => 1,
    '#options' => array('不启用', '立即启用')
  );
  
  $form['fields']['cid'] = array(
    '#type' => 'hidden',
    '#default_value' => !empty($cat->cid) ? $cat->cid : 0,
    '#constant' => 1
  );
  
  $form['fields']['pid'] = array(
    '#type' => 'hidden',
    '#default_value' => !empty($cat->pid) ? $cat->pid : 0,
    '#constant' => 1
  );
  
  $form['fields']['button'] = array(
    '#type' => 'submit',
    '#submit' => 'article_field_category_form_submit'
  );
  
  $form['settings'] = array(
    '#validate' => array('article_field_category_form_validate'),
    '#ajax_validate' => 1
  );
  
  return $form;
}

function article_field_category_form_validate(&$form, &$v) {
  $e = NULL;
  
  if (mb_strlen($v['name']) > 30) {
    $e['name']['#error'][] = '名称长度不超过 30 个字';
  } else if (preg_match('/[^0-9|a-z|_]/i', $v['type'])) {
    $e['type']['#error'][] = '分类索引只能是数字、字母、下划线';
  } else if (strlen($v['type']) > 12) {
    $e['type']['#error'][] = '长度不超过 12 个字符';
  } else if (db_query('SELECT cid FROM {article_cat} WHERE name = ? AND cid != ? AND pid != ?',
  array($v['name'], $v['cid'], $v['pid']), array('return' => 'column'))) {
    $e['name']['#error'][] = '分类名称已经存在，请换一个名称';
  } else if (db_query('SELECT cid FROM {article_cat} WHERE type = ? AND cid != ?',
  array($v['type'], $v['cid']), array('return' => 'column'))) {
    $e['type']['#error'][] = '分类索引已经存在，请换一个索引';
  }
  
  return $e;
}

function article_field_category_form_submit(&$form, &$v) {
  if (!empty($v['cid'])) {
    $args = array('cid');
  } else {
    $args = NULL;
  }
  
  if (db_write_record('article_cat', $v['_dida_values'], $args)) {
    // 更新分类缓存
    article_system_cache_clear('submit', array('value' => array('article_cat' => true)));

    // 清除管理菜单缓存
    cache_del('*', 'admin_menus_');

    dd_get_message('操作成功');

    $form['settings']['#redirect'] = url('admin/article/category');
  }
}


