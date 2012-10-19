<?php
// $Id$

/**
 * @file 资讯浏览模板
 * @param object $article;
 */

?>

<div class="artcile-view article-view-<?php echo $article->nid?>" id="article-view">
  <h2 class="article-view-title"><?php echo $article->title?></h2>
  <div class="article-view-links">
    <?php
      echo '<span class="user">' . t('article', '发布人：!name', array('!name' => theme('username', $article))) . '</span>';
      echo '<span class="date">' . t('article', '发布时间：!time', array('!time' => format_date($article->created))) . '</span>';
      echo '<span class="visit">' . t('article', '访问 !count 次', array('!count' => $article->visit)) . '</span>';
    ?>
  </div>

  <?php if (!empty($article->field_html)) : ?>
  <div class="article-view-meta"><?php echo $article->field_html; ?></div>
  <?php endif; ?>

  <div class="article-view-body"><?php echo $article->body?></div>

  <?php if ($link = article_get_prev_and_next($article->nid, $article->cid)) : ?>
    <div class="article-view-prev-and-next">
    <?php if ($link['prev']) : ?>
      <div class="left">
        <?php echo l(t('article', '上一篇：!title', array('!title' => $link['prev']->title)), 'article/' . $link['prev']->nid)?>
      </div>
    <?php endif?>
    <?php if ($link['next']) : ?>
      <div class="right">
        <?php echo l(t('article', '下一篇：!title', array('!title' => $link['next']->title)), 'article/' . $link['next']->nid)?>
      </div>
    <?php endif?>
    </div>
  <?php endif;?>

</div>
