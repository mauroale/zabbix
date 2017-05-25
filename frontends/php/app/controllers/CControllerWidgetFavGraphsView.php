<?php
/*
** Zabbix
** Copyright (C) 2001-2017 Zabbix SIA
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**/


require_once dirname(__FILE__).'/../../include/blocks.inc.php';

class CControllerWidgetFavGraphsView extends CController {

	protected function init() {
		$this->disableSIDValidation();
	}

	protected function checkInput() {
		$fields = [
			'name' =>		'required|string'
		];

		$ret = $this->validateInput($fields);

		if (!$ret) {
			// TODO VM: prepare propper response for case of incorrect fields
			$this->setResponse(new CControllerResponseData(['main_block' => CJs::encodeJson('')]));
		}

		return $ret;
	}

	protected function checkPermissions() {
		return ($this->getUserType() >= USER_TYPE_ZABBIX_USER);
	}

	protected function doAction() {
		$name = $this->getInput('name');
		if ($name === '') {
			$name = CWidgetConfig::getKnownWidgetTypes()[WIDGET_FAVOURITE_GRAPHS];
		}

		$graphs = [];
		$ids = ['graphid' => [], 'itemid' => []];

		foreach (CFavorite::get('web.favorite.graphids') as $favourite) {
			$ids[$favourite['source']][$favourite['value']] = true;
		}

		if ($ids['graphid']) {
			$db_graphs = API::Graph()->get([
				'output' => ['graphid', 'name'],
				'selectHosts' => ['hostid', 'name'],
				'expandName' => true,
				'graphids' => array_keys($ids['graphid'])
			]);

			foreach ($db_graphs as $db_graph) {
				$graphs[] = [
					'graphid' => $db_graph['graphid'],
					'label' => $db_graph['hosts'][0]['name'].NAME_DELIMITER.$db_graph['name'],
					'simple' => false
				];
			}
		}

		if ($ids['itemid']) {
			$db_items = API::Item()->get([
				'output' => ['itemid', 'hostid', 'name', 'key_'],
				'selectHosts' => ['hostid', 'name'],
				'itemids' => array_keys($ids['itemid']),
				'webitems' => true
			]);

			$db_items = CMacrosResolverHelper::resolveItemNames($db_items);

			foreach ($db_items as $db_item) {
				$graphs[] = [
					'itemid' => $db_item['itemid'],
					'label' => $db_item['hosts'][0]['name'].NAME_DELIMITER.$db_item['name_expanded'],
					'simple' => true
				];
			}
		}

		CArrayHelper::sort($graphs, ['label']);

		$this->setResponse(new CControllerResponseData([
			'graphs' => $graphs,
			'user' => [
				'debug_mode' => $this->getDebugMode()
			],
			'name' => $name
		]));
	}
}
