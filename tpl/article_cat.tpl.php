<?php // $Id$

/**
 * @file 资讯分类显示模板 
 * @param object $cat 
 *  (array) $cat->list - 资讯列表 
 *  (string) $cat->pager - 分页
 */

?>

<div id="article-list-summary">
<?php

if ($cat->list) {
  foreach ($cat->list as $o) {
    echo '<div class="article-list-summary">';
    
    if (!empty($o->files)) {
      echo '<div class="article-list-summary-image">';
      echo '<a href="' . $o->path . '" title="' . $o->title . '">';
      echo img(image_get_thumb($o->files[0]->fid, $o->files[0]->filepath, '150x150'), $o->title, $o->title);
      echo '</a>';
      echo '</div>';
    }
    
    echo '<div class="article-list-summary-content">';
    echo '<h2 class="news-title">';
    echo '<a href="' . $o->path . '" title="' . $o->title . '">' . $o->title . '</a>';
    echo '</h2>';
    
    echo  '<div class="article-list-summary-body">' . $o->summary . '</div>'; // 摘要

    echo '<div class="article-list-summary-links">';
    echo '<span class="date">' . t('article', '发布时间：!time', array('!time' => format_date($o->created))) . '</span>';
    echo '<span class="visit">' . t('article', '访问 !count 次', array('!count' => $o->visit)) . '</span>';
    echo '<span class="more">' . l(t('article', '查看详情'), $o->path) . '</span>';
    echo '</div>';
    
    echo '</div>';
    echo '</div>';
  }
} else {
  echo system_no_content();
}
 
?>

<div class="right"><?php echo $cat->pager?></div>

</div>
