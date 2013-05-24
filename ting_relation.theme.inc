<?php
/**
 * @file
 * Theming functions for ting_relation.
 */

/**
 * Preprocess variables for ting_relation.tpl.php.
 */
function template_preprocess_ting_relation(&$variables) {
  $variables['content'] = array();

  // Get relations from object.
  $relations = $variables['elements']['#object']->getRelations();

  // Group relations by type.
  $all_relations = array();
  foreach ($relations as $rel) {
    $all_relations[$rel->type][] = $rel;
  }

  // Get data from each relation group.
  foreach ($all_relations as $ns => $relations) {
    switch ($ns) {
      case 'dbcaddi:hasReview':
        $variables['content'][$ns] = ting_relation_reviews($relations);
        break;
      case 'dbcaddi:hasSubjectDescription':
        $variables['content'][$ns] = ting_relation_subjects($relations);
        break;
      case 'dbcaddi:hasCreatorDescription':
        $variables['content'][$ns] = ting_relation_creator_description($relations);
        break;
      case 'dbcaddi:hasOnlineAccess':
        $cont = ting_relation_online($relations);
        if (!empty($cont)) {
          $variables['content'][$ns] = $cont;
        }
        break;
      case 'isPartOfManifestation':
        $variables['content'][$ns] = ting_relation_ispartofmanifestation($relations);
      default:
        break;
    }
  }
}

/**
 * two variants(sofar) Ebrary and filmstriben
 */
function ting_relation_online($relations) {
  $ret = array();
  foreach ($relations as $relation) {
    // TODO Check [type] => dbcaddi:hasOnlineAccess, [uri] => [useraccessinfomedia]?action= ...
    if (!is_object($relation->object)) {
      continue;
    }
    $ac_source = $relation->object->getAcSource();
    if ($ac_source == 'Ebrary') {
      $ret[] = ting_relation_online_ebrary($relation);
    }
    elseif ($ac_source == 'Filmstriben (fjernlå)') {
      $ret[] = ting_relation_online_filmstriben($relation);
    }
  }
  return $ret;
}

/**
 * a variant(Ebrary) of 'hasOnlineAccess'
 */
function ting_relation_online_ebrary($relation) {
  $ebrary_url = variable_get('ting_ebrary_url');
  if (!isset($ebrary_url)) {
    return;
  }

  $rel = $relation['#relation'];
  $content['type'] = $relation['#title'];
  // Simply set a button.
  $url = str_replace('[URL]', $ebrary_url, $rel->uri);
  $content['online_url'] = array('#markup' => l($relation['#ac_source'], $url, array('attributes' => array('target' => '_blank'))));

  return $content;
}

/**
 * a variant(filmstriben) of 'hasOnlineAccess'
 */
function ting_relation_online_filmstriben($relation) {
  $rel = $relation['#relation'];
  $content['type'] = $relation['#title'];
  // Simply set a button.
  $url = $rel->uri;
  $content['online_url'] = array('#markup' => l($relation['#ac_source'], $url, array('attributes' => array('target' => '_blank'))));
  return $content;
}

function ting_relation_creator_description($relations) {
  foreach ($relations as $relation) {
    $content = array();
    $ting = $relation->object;

    $content['type'] = _ting_relation_title($relation->type);
    $content['title'] = $ting->getTitle();

    $creators = $ting->getCreators();
    if (!empty($creators)) {
      $variables['content']['byline'] = ting_relation_array2names($creators);
    }

    $content['abstract'] = filter_xss_admin($ting->getAbstract());

    // online_url.
    $url = $ting->getOnlineUrl();
    if (!empty($url)) {
      $content['online_url'] = array('#markup' => l($ting->getAcSource(), $url, array('attributes' => array('target' => '_blank'))));
    }

    $ret[] = $content;
  }
  return $ret;
}

function ting_relation_ispartofmanifestation($relations) {

  $ret = array();
  foreach ($relations as $relation) {
    $content = array();
    $content['type'] = _ting_relation_title($relation->type);
    $related_object = $relation->object;
    $rendered_item = ting_object_view($related_object, 'collection_objects');
    $content['text'] = render($rendered_item);
    $ret[] = $content;
  }

  return $ret;
}

function ting_relation_subjects($relations) {
  foreach ($relations as $relation) {
    $content = array();
    $ting = $relation->object;

    $content['type'] = _ting_relation_title($relation->type);
    $content['title'] = $ting->getTitle();

    $creators = $ting->getCreators();
    if (!empty($creators)) {
      $content['byline'] = t('by @author', array('@author' => ting_relation_array2names($creators)));
    }

    $content['abstract'] = filter_xss_admin($ting->getAbstract());
    $content['text'] = filter_xss_admin($ting->getDescription());

    $url = $ting->getOnlineUrl();
    if (!empty($url)) {
      $content['online_url'] = array('#markup' => l($ting->getAcSource(), $url, array('attributes' => array('target' => '_blank'))));
    }

    $ret[] = $content;
  }

  return $ret;
}

/**
 * two variants sofar - review(ac:source=Anmeldelser) and all the others (sofar only ac:source=bibliotekets materialer)
 */
function ting_relation_reviews($relations) {
  $ret = NULL;
  foreach ($relations as $relation) {
    $ac_source = NULL;
    if (is_object($relation->object)) {
      $ac_source = $relation->object->getAcSource();
    }

    if ($ac_source == 'Anmeldelser') {
      $ret[] = ting_relation_review_other($relation);
    }
    else {
      $ret[] = ting_relation_review($relation);
    }
  }
  return $ret;
}

/**
 * a variant of a review (ac:source = 'bibliotekets materialer'
 */
function ting_relation_review_other($relation) {
  $ting = $relation->object;

  // Type title.
  $content['type'] = _ting_relation_title($relation->type);

  // Title.
  global $base_url;

  // Check if we should link to fulltext.
  if (in_array('docbook', $ting->getFormatsAvailable()) && !$ting->getIndentifier()) {
    $content['title'] = l($ting->getTitle(), $base_url . '/ting/object/' . $ting->ding_entity_id . '/fulltext');
  }
  else{
    $content['title'] = l($ting->getTitle(), $base_url . '/ting/object/' . $ting->ding_entity_id);
  }
  // Creators.
  $names = ting_relation_array2names($ting->getCreators());
  if (!empty($names)) {
    $content['byline'] = t('by @author', array('@author' => $names));
  }
  // Ispartof.
  $part_of = ting_relation_array2names($ting->getIsPartOf());
  if (!empty($part_of)) {
    $extent = $ting->getExtent();
    $content['isPartOf'] = filter_xss_admin($part_of . ', ' . $extent);
  }

  // Online_url.
  $url = $ting->getOnlineUrl();
  if (!empty($url)) {
    $content['online_url'] = array('#markup' => l($ting->getAcSource(), $url, array('attributes' => array('target' => '_blank'))));
  }

  return $content;
}

/**
 * a variant of a review (ac:source = 'Anmeldelser'
 */
function ting_relation_review($relation) {
  $ting = $relation->object;

  // Type title ; generel.
  $content['type'] = _ting_relation_title($relation->type);

  // Title ; generel.
  global $base_url;
  // Check if we should link to fulltext.
  if (in_array('docbook', $ting->getFormatsAvailable()) && !$ting->getIdentifier()) {
    $content['title'] = l($ting->getTitle(), $base_url . '/ting/object/' . $ting->ding_entity_id . '/fulltext');
  }
  else{
    $content['title'] = l($ting->getTitle(), $base_url . '/ting/object/' . $ting->ding_entity_id);
  }
  // Year.
  $content['year'] = $ting->getDate();
  // Creator.
  $content['creator'] = ting_relation_array2names($ting->getCreators());
  // Text.
  $content['text'] = $ting->getDescription();


  $url = $ting->getOnlineUrl();
  if (!empty($url)) {
    $content['online_url'] = array('#markup' => l($ting->getAcSource(), $url, array('attributes' => array('target' => '_blank'))));
  }

  return $content;
}

/**
 * helper function
 */
function ting_relation_array2names($array) {
  // Input is empty.
  if (empty($array)) {
    return FALSE;
  }

  // It is string, so return it directly.
  if (!is_array($array)) {
    return $array;
  }

  // Merge array.
  $ret = '';
  foreach ($array as $key => $name) {
    if (strlen($ret) > 0) {
      $ret .= ', ';
    }
    $ret .= $name;
  }
  return $ret;
}
