<?php
/**
 * @file
 * Template to render ting_releation content.
 */
if (is_array($content)) {
  foreach ($content as $ns => $relations) {
    if (!empty($relations) && $ns != 'dbcaddi:hasOnlineAccess') { ?>
    <div id="<?php echo $relations[0]['id']; ?>" class="<?php print $classes . ' field-group-format group_relations ting-relation-' . drupal_html_class($ns) . ' clearfix'; ?>">
      <fieldset class="field-group-fieldset group-relations collapsible collapsed">
      <legend>
        <span class="fieldset-legend"><?php print ucfirst($relations[0]['type']); ?></span>
      </legend>
      <div class="fieldset-wrapper">
      <?php foreach ($relations as $relation): ?>
        <div class="meta">
          <?php
          if (isset($relation['title'])) {
            print '<h3>' . $relation['title'] . '</h3>';
          }
          if (isset($relation['creator'])) {
            print '<div>' . $relation['creator'];
            if (isset($relation['year'])) {
              print '<span> (' . $relation['year'] . ')</span>';
            }
            print '</div>';
          }
          if (isset($relation['byline'])) {
            print '<div>' . $relation['byline'] . '</div>';
          }
          if (isset($relation['isPartOf'])) {
            print $relation['isPartOf'];
          }
          if (isset($relation['abstract'])) {
            print '<div>' . $relation['abstract'] . '</div>';
          }
          if (isset($relation['text'])) {
            print '<div>' . $relation['text'] . '</div>';
          }
          if (!empty($relation['online_url'])) {
            print '<div class="field-type-ting-relation">';
            print '<div class="field-items rounded-corners">';
            $online_url = $relation['online_url'];
            print render($online_url);
            print '</div></div>';
          }
          if (!empty($relation['docbook_link'])) {
            $docbok_link = $relation['docbook_link'];
            print render($docbok_link);
          }
          ?>
        </div>
        <?php

        print '<div class="clearfix"></div>';
      endforeach;
      print '</div></fieldset></div>';
    }
  }
}
