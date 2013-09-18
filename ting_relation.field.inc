<?php

/**
 * @file
 * Field hook implementations.
 */

/**
 * Implements hook_field_info().
 */
function ting_relation_field_info() {
  return array(
    'ting_relations' => array(
      'label' => t('Ting relations.'),
      'description' => t('Ting relations.'),
      'default_widget' => 'hidden',
      'default_formatter' => 'ting_relations_default',
      'no_ui' => TRUE,
    ),
  );
}

/**
 * Implements hook_field_load().
 */
function ting_relation_field_load($entity_type, $entities, $field, $instances, $langcode, &$items, $age) {
  foreach ($entities as $id => $entity) {
    // We could create relations as multiple values here, but it's
    // cached after this hook, so we don't.
    $items[$id][0] = array(
      'id' => $id,
    );
  }
}

/**
 * Implements hook_widget_info_alter().
 */
function ting_relation_widget_info_alter(&$info) {
  if (isset($info['hidden'])) {
    $info['hidden']['field types'][] = 'ting_relations';
  }
}

/**
 * Implements hook_field_formatter_info().
 */
function ting_relation_field_formatter_info() {
  return array(
    'ting_relations_default' => array(
      'label' => t('Default'),
      'field types' => array(
        'ting_relations',
      ),
    ),
  );
}

/**
 * Implements hook_field_formatter_view().
 */
function ting_relation_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $relations_array = array();
  $relations_types = ting_relation_inline();

  foreach ($entity->getRelations() as $relation) {
    if ($relation->type == 'isPartOfManifestation' && $relation->object->getOwnerId() !== variable_get('ting_agency', NULL)) {
      continue;
    }

    if (isset($relations_types[$relation->type])) {
      $object = $relation->object;

      $relations_array[$relation->type][$object->getLocalId()] = array(
        '#ac_source' => $entity->getAcSource(),
        '#relation' => $relation,
        '#object' => $relation->object,
        '#title' => $relation->type,
      );
    }
  }

  $element = array(
    '#theme' => 'ting_relation',
    '#relations' => $relations_array,
    '#attached' => array(
      'css' => array(drupal_get_path('module', 'ting_relation') . '/ting_relation.css'),
      'library' => array('form', 'collapse'),
    ),
  );

  return $element;
}
