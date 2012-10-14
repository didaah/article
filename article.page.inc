<?php
// $Id$

/**
 * 文章浏览
 * @param object $article
 */
function article_page_view($article) {
  if (empty($article->status)) dd_get_access();
  
  dd_set_title(array($article->title, $article->cat->name));
  
  db_exec('UPDATE {article} SET visit = visit + 1 WHERE nid = ?', array($article->nid));
  
  dd_set_breadcrumb(array(
    l($article->cat->name, 'article/category/' . $article->cid),
    $article->title
  ));

  module_invoke_all('article_view', $article);

  field_get_node_view('article', 'default', $article, 2);

  if (!empty($article->cat)) {
    field_get_node_view('article', 'ext_article_' . $article->cat->type, $article, 2);
  }

  $article->field_html = field_get_node_html('article', $article);
  
  dd_add_css(dd_get_path('module', 'article') . '/article.css');

  return theme('article_view', $article);
}

/**
 * 分类浏览
 */
function article_page_category($cat) {
  dd_set_title(array($cat->name));
  dd_set_breadcrumb(array($cat->name));
  return article_list_summary(0, 0, $cat->cid);
}


