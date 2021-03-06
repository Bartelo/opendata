<?php

/** 
 * Creates a view display
 * 
 * @param $form_state 
 *    An array holding the form data 
 * 
 * @return 
 *    A $form array with user provided values.
 *
 */
function opendata_create_display(&$view, $type, &$form_state) {
  
  $data_table_id = $form_state['storage']['general_values']['details']['data_table'];
  $data_table_name = _opendata_get_tablename($data_table_id);
  $office = _opendata_formatter($form_state['storage']['general_values']['details']['office'], 'lower');
  $fiscal_year = $form_state['storage']['general_values']['details']['fiscal_year'];
  $full_title =  $form_state['storage']['general_values']['details']['title'];
  $title =  _opendata_formatter($form_state['storage']['general_values']['details']['title'], 'hyphen');
  //$description = _opendata_formatter($form_state['storage']['general_values']['details']['description'], 'html');
  $desc_brief = _opendata_formatter($form_state['storage']['general_values']['details']['desc_brief'], 'html');
  $desc_extended = _opendata_formatter($form_state['storage']['general_values']['details']['desc_extended'], 'html');
  $more_info_url = $form_state['storage']['general_values']['details']['more_info_url'];

  $underscored_title = _opendata_formatter($form_state['storage']['general_values']['details']['title'], 'underscore');
  $unique_name = substr('od_' . $office . '_' . $fiscal_year . '_' . $underscored_title, 0, 30);

  $url = 'grants/' . $office . '/' . $fiscal_year . '/' . $title;

  $page_limit = $form_state['storage']['summary_values']['page_limit'];
  $table_key = $form_state['storage']['summary_values']['table_key'];
  $map_title = $form_state['storage']['summary_values']['summary_map']['map_title'];
  $map_sub_title = $form_state['storage']['summary_values']['summary_map']['map_sub_title'];
  $map_lat = $form_state['storage']['summary_values']['summary_map']['map_lat'];
  $map_long = $form_state['storage']['summary_values']['summary_map']['map_long'];

	// check if "Include Map" has been checked for summary and detail pages
	$do_map_summary = $form_state['storage']['summary_values']['summary_map']['map_option'];
	$do_map_detail = $form_state['storage']['published_values']['map_option'];

  // get the list of column names
  $colnames = array();
  $colnames = _opendata_get_fields($data_table_id);

  // save relevant field names for the map data
  $other_lat = $colnames[$map_lat];
  $other_lon = $colnames[$map_long];
  $name_field = $colnames[$map_title];
  $description_field = $colnames[$map_sub_title];

  // get the name of the selected table key field
	// we no longer need to get the name, the var already contains the name
  //$table_key = $colnames[$table_key];

  // get an array of the detail page fields
  $detail_fields = array();
  $detail_fields = $form_state['storage']['published_values']['detail_fields'];

	$detail_title_raw = $colnames[$form_state['storage']['published_values']['detail_title_field']];
	//$detail_title_raw = _opendata_get_title_field($colnames, $detail_fields);
	$detail_title = _opendata_formatter($detail_title_raw, 'space');

	// set the detail perm url based on the selected table key field
	$detail_url = $url . '/detail/[' . $table_key . '_arg' . ']/[' . $detail_title_raw . '_arg' . ']';
  
  //Get array of the summary fields
  $summary_fields = array();
  $summary_fields = $form_state['storage']['summary_values']['summary_fields'];

  // get selected fields
	// new modifiction, add the detail_title field as the first or second field (after id field) 
  $selected_fields = array();
  $selected_fields = _opendata_build_view_summary_fields($data_table_name, $colnames, $summary_fields, $detail_url, $table_key, $detail_title_raw);

  // get selected fields
  $filter_fields = array();
  $filter_fields = _opendata_build_view_filter_fields($data_table_name, $colnames, $summary_fields, $detail_url, $table_key);

	//===========================================================================
	// begin display construction
	//===========================================================================
	if ($type == 'default') {
		$handler = $view->new_display('default', $full_title . ' default', 'default');
		$handler->override_option('fields', $selected_fields);
	  $handler->override_option('filters', $filter_fields);
		$handler->override_option('access', array(
			'type' => 'perm',
			'perm' => 'table wizard administration',
		));
	
		$handler->override_option('cache', array(
			'type' => 'none',
		));
	
		$handler->override_option('header_format', '1');
		$handler->override_option('header_empty', 0);
		$handler->override_option('empty', 'There are no rows in this table.');
		$handler->override_option('empty_format', '1');
		$handler->override_option('items_per_page', $page_limit);
		$handler->override_option('use_pager', '1');
		$handler->override_option('style_plugin', 'table');
	
		// build the style options
		//TODO: work with _opendata_build_style_options
		$style_options = array();
		$style_options = _opendata_build_style_options($colnames, $summary_fields);
		$handler->override_option('style_options', $style_options);
		$handler->override_option('row_plugin', '');
		$handler->override_option('displays', array());
	} elseif ($type == 'desc_block') {
		$handler = $view->new_display('block', $title . ' description block', 'block_1');
		$desc_fields = _opendata_build_desc_fields($more_info_url, $desc_brief, $desc_extended); 
		$handler->override_option('fields', $desc_fields);
    $handler->override_option('filters', array());
		$handler->override_option('header', '');
		$handler->override_option('header_format', '3');
		$handler->override_option('items_per_page', 1);
		$handler->override_option('use_pager', '0');
		$handler->override_option('style_plugin', 'default');
		$handler->override_option('style_options', NULL);
		$handler->override_option('row_plugin', 'fields');
		$handler->override_option('block_description', '');
		$handler->override_option('block_caching', -1);
	} elseif ($type == 'page_summary') {
		$url = 'grants/' . $office . '/' . $fiscal_year . '/' . $title;
		$handler = $view->new_display('page', $full_title . ' summary listing page', 'page_1');
		$handler->override_option('title', $full_title);
		$handler->override_option('footer', '<script type="text/javascript">
		var lnk = \'<span class="breadcrumb-link"><a href="<?php global $base_path; print $base_path; ?>">data.ed.gov</a></span>\';
		lnk += \'<span class="breadcrumb-link"><strong><?php print arg(3); ?></strong></span>\';
		$(\'#main .breadcrumb\').html(lnk);
		</script>');
		$handler->override_option('footer_format', '3');
		$handler->override_option('footer_empty', 0);
		$handler->override_option('path', $url);
		$handler->override_option('menu', array(
			'type' => 'none',
			'title' => '',
			'description' => '',
			'weight' => 0,
			'name' => 'navigation',
		));
		$handler->override_option('tab_options', array(
			'type' => 'none',
			'title' => '',
			'description' => '',
			'weight' => 0,
			'name' => 'navigation',
		));
	} elseif ($type == 'openlayers_data_summary') {
		$handler = $view->new_display('openlayers', $full_title . ' summary map data', 'openlayers_1');
		$map_fields = array();
		$a1 = $data_table_name; 
		$a2 = $other_lat;
		$a3 = $other_lon; 
		$a4 = $name_field; 
		$a5 = $description_field;
		$a6 = $table_key;
		$a7 = $detail_url;
		$a8 = $detail_title_raw;
		$map_fields = _opendata_build_map_fields($a1, $a2, $a3, $a4, $a5, $a6, $a7, $a8);
    $handler->override_option('fields', $map_fields);
		$handler->override_option('items_per_page', 0);
		$handler->override_option('style_plugin', 'openlayers_data');
		$handler->override_option('style_options', array(
			'grouping' => '',
			'data_source' => array(
				'value' => array('other_latlon' => 'other_latlon',),
				'other_lat' => $other_lat,
				'other_lon' => $other_lon, 
				'openlayers_wkt' => $table_key,
				'other_top' => $table_key,
				'other_right' => $table_key,
				'other_bottom' => $table_key,
				'other_left' => $table_key,
				'name_field' => $name_field,
				'description_field' => $description_field,
			),
		));
	} elseif ($type == 'openlayers_data_detail') {
		$handler = $view->new_display('openlayers', $full_title . ' detail map data', 'openlayers_2');
		$map_fields = array();
		$a1 = $data_table_name; 
		$a2 = $other_lat;
		$a3 = $other_lon; 
		$a4 = $name_field; 
		$a5 = $description_field;
		$a6 = $table_key;
		$a7 = $detail_url;
		$a8 = $detail_title_raw;
		$map_fields = _opendata_build_map_fields($a1, $a2, $a3, $a4, $a5, $a6, $a7, $a8);
    $handler->override_option('fields', $map_fields);
		$handler->override_option('filters', array());
		$handler->override_option('items_per_page', 0);
		$handler->override_option('style_plugin', 'openlayers_data');
		$handler->override_option('style_options', array(
			'grouping' => '',
			'data_source' => array(
				'value' => array('other_latlon' => 'other_latlon',),
				'other_lat' => $other_lat,
				'other_lon' => $other_lon, 
				'openlayers_wkt' => $table_key,
				'other_top' => $table_key,
				'other_right' => $table_key,
				'other_bottom' => $table_key,
				'other_left' => $table_key,
				'name_field' => $name_field,
				'description_field' => $description_field,
			),
		));
		$handler->override_option('arguments', array(
			$table_key => array(
				'default_action' => 'default',
				'style_plugin' => 'default_summary',
				'style_options' => array(),
				'wildcard' => 'all',
				'wildcard_substitution' => 'All',
				'title' => '',
				'breadcrumb' => '',
				'default_argument_type' => 'php',
				'default_argument' => '',
				'validate_type' => 'none',
				'validate_fail' => 'not found',
				'glossary' => 0,
				'limit' => '0',
				'case' => 'none',
				'path_case' => 'none',
				'transform_dash' => 0,
				'id' => $table_key,
				'table' => $data_table_name,
				'field' => $table_key,
				'validate_user_argument_type' => 'uid',
				'validate_user_roles' => array(
					'2' => 0,
					'3' => 0,
				),
				'override' => array(
					'button' => 'Use default',
				),
				'relationship' => 'none',
				'default_options_div_prefix' => '',
				'default_argument_fixed' => '',
				'default_argument_user' => 0,
				'default_argument_php' => 'return arg(5);',
				'validate_argument_node_type' => array(
					'webform' => 0,
					'rotor_item' => 0,
					'application' => 0,
					'blog' => 0,
					'data' => 0,
					'pnapp' => 0,
					'pn' => 0,
					'cag' => 0,
					'cag_schools' => 0,
					'charter_school' => 0,
					'ope_grant_desc' => 0,
					'page' => 0,
					'rtl' => 0,
					'tah_losers' => 0,
					'tah_winners' => 0,
				),
				'validate_argument_node_access' => 0,
				'validate_argument_nid_type' => 'nid',
				'validate_argument_vocabulary' => array(
					'1' => 0,
					'11' => 0,
					'2' => 0,
					'12' => 0,
					'3' => 0,
					'14' => 0,
					'4' => 0,
					'13' => 0,
					'16' => 0,
					'15' => 0,
					'17' => 0,
					'5' => 0,
				),
				'validate_argument_type' => 'tid',
				'validate_argument_transform' => 0,
				'validate_user_restrict_roles' => 0,
				'validate_argument_php' => '',
			),
		));
	} elseif ($type == 'map_block_detail') {
		$handler = $view->new_display('block', $full_title . ' detail map block', 'block_3');
		$handler->override_option('items_per_page', 0);
		$handler->override_option('use_pager', '0');
		$handler->override_option('style_plugin', 'openlayers_map');
		$handler->override_option('style_options', array(
		 'preset' => $unique_name . '_detail',
		));
		$handler->override_option('block_description', '');
		$handler->override_option('block_caching', -1);
	} elseif ($type == 'map_block') {
		$handler = $view->new_display('block', $full_title . ' summary map block', 'block_2');
		$handler->override_option('items_per_page', 0);
		$handler->override_option('use_pager', '0');
		$handler->override_option('style_plugin', 'openlayers_map');
		$handler->override_option('style_options', array(
		 'preset' => $unique_name,
		));
		$handler->override_option('block_description', '');
		$handler->override_option('block_caching', -1);
	} elseif ($type == 'csv_block') {
		// there's a possibility that the user might check the csv download box 
		// but not mark any of the summary fields as exportable. in this case, 
		// we should just export the default fields

    $handler = $view->new_display('feed', $full_title . ' csv block', 'feed_1');

		$csv_fields = array();
		$a1 = $data_table_name; 
		$a2 = $colnames;
		$a3 = $summary_fields;
		$a4 = $table_key;
		$csv_fields = _opendata_build_csv_fields($a1, $a2, $a3, $a4);
    $handler->override_option('fields', $csv_fields);

    $handler->override_option(
        'items_per_page', 0
    );
    $handler->override_option('style_plugin', 'views_csv');
    $handler->override_option('style_options', array(
      'mission_description' => FALSE,
      'description' => '',
      'attach_text' => 'CSV',
      'provide_file' => 1,
      'filename' => $unique_name . '.csv',
      'parent_sort' => 0,
      'seperator' => ',',
      'quote' => 1,
      'header' => 1,
      )
    );
    $handler->override_option('row_plugin', '');
    $handler->override_option('path', $url . '/csv');
    
    $handler->override_option('displays', array(
      'page_1' => 'page_1',
      'default' => 0,
      'block_1' => 0,
    ));
  } elseif ($type == 'page_detail') {
    $handler = $view->new_display('page', $full_title . ' detail page', 'page_2');
    
    // build detail page fields
    $selected_fields = array();
    $a1 = $data_table_name;
    $a2 = $colnames;
    $a3 = $detail_fields;
    $a4 = $table_key;
		$a5 = $do_map_detail;

    $selected_fields = _opendata_build_detail_view_fields($a1, $a2, $a3, $a4, $a5);

    $handler->override_option('fields', $selected_fields);

    $handler->override_option('arguments', array(
      $table_key => array(
				'default_action' => 'not found',
				'style_plugin' => 'default_summary',
				'style_options' => array(),
				'wildcard' => 'all',
				'wildcard_substitution' => 'All',
				'title' => '',
				'breadcrumb' => '',
				'default_argument_type' => 'fixed',
				'default_argument' => '',
				'validate_type' => 'none',
				'validate_fail' => 'not found',
				'break_phrase' => 0,
				'not' => 0,
        'id' => $table_key,
        'table' => $data_table_name,
        'field' => $table_key,
        'relationship' => 'none',
      ),
      $detail_title_raw => array(
				'default_action' => 'default',
				'style_plugin' => 'default_summary',
				'style_options' => array(),
				'wildcard' => 'all',
				'wildcard_substitution' => $full_title,
				'title' => '%2',
				'breadcrumb' => '',
				'default_argument_type' => 'fixed',
				'default_argument' => '',
				'validate_type' => 'none',
				'validate_fail' => 'not found',
				'glossary' => 0,
				'limit' => '0',
				'case' => 'none',
				'path_case' => 'none',
				'transform_dash' => 0,
        'id' => $detail_title_raw,
        'table' => $data_table_name,
        'field' => $detail_title_raw,
        'relationship' => 'none',
				'default_argument_fixed' => 'all',
      ),
    ));
		$handler->override_option('filters', array());
		$handler->override_option('title', $full_title);
		$handler->override_option('footer', '<script type="text/javascript">
		var lnk = \'<span class="breadcrumb-link"><a href="<?php global $base_path; print $base_path; ?>">data.ed.gov</a></span>\';
		lnk += \'<span class="breadcrumb-link"><a href="<?php print $base_path . arg(0) . \'/\' . arg(1) . \'/\' . arg(2) . \'/\' . arg(3); ?>"><?php print arg(3); ?></a></span>\';
		lnk += \'<span class="breadcrumb-link"><strong><?php print arg(6); ?></strong></span>\';
		$(\'#main .breadcrumb\').html(lnk);
		</script>');
		$handler->override_option('footer_format', '3');
		$handler->override_option('footer_empty', 0);
		$handler->override_option('style_plugin', 'default');
		$handler->override_option('style_options', NULL);
		$handler->override_option('row_plugin', 'fields');
    $handler->override_option('path', $url . '/detail/%');
		$handler->override_option('menu', array(
			'type' => 'none',
			'title' => '',
			'description' => '',
			'weight' => 0,
			'name' => 'navigation',
		));
		$handler->override_option('tab_options', array(
			'type' => 'none',
			'title' => '',
			'description' => '',
			'weight' => 0,
			'name' => 'navigation',
		));
	} 
	return $form;
}
