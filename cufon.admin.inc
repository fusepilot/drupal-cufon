<?php
// $Id$

function cufon_form($form, &$form_state) {	
	if(count(_cufon_get_fonts()) == 0) {
		drupal_set_message('Add Cufon font files to "/sites/all/libraries/cufon".');
	}
	
	
	$form['#tree'] = TRUE;
	
	//run only the first time
	if(!isset($form_state['replacements_data'])) {
		//get replacments from db
		$replacments = db_select('cufon_replacements')
		->fields('cufon_replacements')
		->execute()
		->fetchAll();
		
		//transfer replacements over to the state 
		foreach($replacments as $i => $replacement) {
			$replacement->id = rand();
			$form_state['replacements_data'][$i] = $replacement;
		}
	}
	
	$form_state['replacements_num'] = isset($form_state['replacements_data']) ? count($form_state['replacements_data']) : 0;
	
	//add fieldset
	$form['cufon_fieldset'] = array(
		'#type' => 'fieldset',
		'#title' => t('Replacements (!n)', array('!n' => $form_state['replacements_num'])),
		'#description' => t('Add Cufon replacements.'),
		'#prefix' => '<div id="cufon-fieldset-wrapper">',
		'#suffix' => '</div>',
	);
	
	//build and add them to the form
	if(isset($form_state['replacements_data'])) {
		foreach($form_state['replacements_data'] as $i => $replacement_data) {
			
			//add to the form 
			$form['cufon_fieldset']['replacements'][] = build_form_relacement($replacement_data);
		}
	}
	
	//add 'add replacement' button
	$form['cufon_fieldset']['add_replacement'] = array(
		'#type' => 'submit',
		'#value' => $form_state['replacements_num'] > 0 ? t('Add Another Replacement') : t('Add Replacement'),
		'#submit' => array('cufon_add_replacement'),
	);
	
	$form['#attached'] = array(
    'css' => array(drupal_get_path('module', 'cufon') . '/cufon.admin.css'),
  );
	
	//handle submit
	$form['#submit'][] = 'cufon_form_submit';
	
	return system_settings_form($form);
}

function build_form_relacement($data) {
	$replacement = array();
	
	$replacement['replacement'] = array(
		'#type' => 'fieldset',
		'#title' => t('Replacement'),
	);
	
	$replacement['replacement']['selectors'] = array(	
		'#type' => 'textfield',
		'#title' => t("Selector"),
		'#size' => 120,
		'#default_value' => isset($data->selectors) ? $data->selectors : '',
	);

	$replacement['replacement']['font'] = array(	
		'#type' => 'select',
		'#title' => t("Font"),
		'#options' => _cufon_get_font_options(),
		'#default_value' => isset($data->font) ? $data->font : '',
	);
	
	$replacement['replacement']['hover'] = array(	
		'#type' => 'checkbox',
    '#title' => t('Hover'),
    '#default_value' => isset($data->font) ? $data->font : 0,
	);
	
	$replacement['replacement']['remove'] = array(	
		'#type' => 'submit',
    '#value' => t('Remove'),
		'#submit' => array('cufon_remove_replacement'),
		'#attributes' => array('data-id' => $data->id),
	);
	
	return $replacement;
}

function cufon_add_replacement($form, &$form_state) {
	$replacement = new stdClass();
	$replacement->id = rand();
	
	$form_state['replacements_data'][] = $replacement;
	
  $form_state['rebuild'] = TRUE;
}

function cufon_remove_replacement($form, &$form_state) {
	if (isset($form_state['clicked_button'])) {
		$id = $form_state['clicked_button']['#attributes']['data-id'];
		
		foreach ($form_state['replacements_data'] as $i => $replacement) {
			if($replacement->id == $id) {
				unset($form_state['replacements_data'][$i]);
			}
		}
	}

  $form_state['rebuild'] = TRUE;
}

function cufon_form_submit($form, &$form_state) {	
	//delete existing
	db_delete('cufon_replacements')
	->execute();
	
	if(isset($form_state['values']['cufon_fieldset']['replacements'])) {
		//repopulate with new and modified entries
		foreach($form_state['values']['cufon_fieldset']['replacements'] as $i => $replacement) {
			db_insert('cufon_replacements')
			->fields(array(
				'selectors' => $form_state['values']['cufon_fieldset']['replacements'][$i]['replacement']['selectors'],
				'font' => $form_state['values']['cufon_fieldset']['replacements'][$i]['replacement']['font'],
				'hover' => $form_state['values']['cufon_fieldset']['replacements'][$i]['replacement']['hover'],
			))
			->execute();
		}
	}
}