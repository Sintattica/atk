<?php
atkConfig::setGlobal('throw_exception_on_error', true);
atkConfig::setGlobal('mailreport', '');

atkClassLoader::mountClassPath('tests', atkconfig('testroot'));
atkClassLoader::addOverloader('atk.session.atksessionmanager', 'atk.test.mocks.atkmocksessionmanager');
atkClassLoader::addOverloader('atk.session.atksecuritymanager', 'atk.test.mocks.atkmocksecuritymanager');  
atksession();
