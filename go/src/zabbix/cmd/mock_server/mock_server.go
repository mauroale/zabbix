/*
** Zabbix
** Copyright (C) 2001-2019 Zabbix SIA
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

package main

import (
	"flag"
	"fmt"
	"io/ioutil"
	"time"
	"zabbix/pkg/comms"
)

func main() {
	var fFlag string
	const (
		fDefault     = "active_checks.json"
		fDescription = "Path to the json file used in response"
	)
	flag.StringVar(&fFlag, "f", fDefault, fDescription)
	flag.Parse()

	dat, err := ioutil.ReadFile(fFlag)
	if err != nil {
		fmt.Printf("Cannot read file: %s\n", err)
		return
	}

	var c comms.ZbxConnection

	err = c.ListenAndAccept(":10051")
	defer c.Close()
	if err != nil {
		fmt.Printf("Listen and accept failed: %s\n", err)
		return
	}

	err = c.Write(dat, time.Second*5)
	if err != nil {
		fmt.Printf("Write failed: %s\n", err)
		return
	}
}
