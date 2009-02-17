<?php

/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.35
 * 
 * This file is not intended to be easily readable and contains a number of 
 * coding conventions designed to improve portability and efficiency. Do not make
 * changes to this file unless you know what you are doing--modify the SWIG 
 * interface file instead. 
 * ----------------------------------------------------------------------------- */

// Try to load our extension if it's not already loaded.
if (!extension_loaded("ESL")) {
  if (strtolower(substr(PHP_OS, 0, 3)) === 'win') {
    if (!dl('php_ESL.dll')) return;
  } else {
    // PHP_SHLIB_SUFFIX is available as of PHP 4.3.0, for older PHP assume 'so'.
    // It gives 'dylib' on MacOS X which is for libraries, modules are 'so'.
    if (PHP_SHLIB_SUFFIX === 'PHP_SHLIB_SUFFIX' || PHP_SHLIB_SUFFIX === 'dylib') {
      if (!dl('ESL.so')) return;
    } else {
      if (!dl('ESL.'.PHP_SHLIB_SUFFIX)) return;
    }
  }
}



/* PHP Proxy Classes */
class eslEvent {
	public $_cPtr=null;

	function __set($var,$value) {
		$func = 'eslEvent_'.$var.'_set';
		if (function_exists($func)) call_user_func($func,$this->_cPtr,$value);
	}

	function __isset($var) {
		return function_exists('eslEvent_'.$var.'_set');
	}

	function __get($var) {
		$func = 'eslEvent_'.$var.'_get';
		if (function_exists($func)) return call_user_func($func,$this->_cPtr);
		return null;
	}

	function __construct($type_or_wrap_me,$subclass_name_or_free_me=null) {
		switch (func_num_args()) {
		case 1: $r=new_eslEvent($type_or_wrap_me); break;
		default: $r=new_eslEvent($type_or_wrap_me,$subclass_name_or_free_me);
		}
		$this->_cPtr=$r;
	}

	function serialize($format=null) {
		switch (func_num_args()) {
		case 0: $r=eslEvent_serialize($this->_cPtr); break;
		default: $r=eslEvent_serialize($this->_cPtr,$format);
		}
		return $r;
	}

	function setPriority($priority=null) {
		switch (func_num_args()) {
		case 0: $r=eslEvent_setPriority($this->_cPtr); break;
		default: $r=eslEvent_setPriority($this->_cPtr,$priority);
		}
		return $r;
	}

	function getHeader($header_name) {
		return eslEvent_getHeader($this->_cPtr,$header_name);
	}

	function getBody() {
		return eslEvent_getBody($this->_cPtr);
	}

	function getType() {
		return eslEvent_getType($this->_cPtr);
	}

	function addBody($value) {
		return eslEvent_addBody($this->_cPtr,$value);
	}

	function addHeader($header_name,$value) {
		return eslEvent_addHeader($this->_cPtr,$header_name,$value);
	}

	function delHeader($header_name) {
		return eslEvent_delHeader($this->_cPtr,$header_name);
	}
}

class eslConnection {
	public $_cPtr=null;

	function __construct($host_or_socket,$port=null,$password=null) {
		switch (func_num_args()) {
		case 1: $r=new_eslConnection($host_or_socket); break;
		case 2: $r=new_eslConnection($host_or_socket,$port); break;
		default: $r=new_eslConnection($host_or_socket,$port,$password);
		}
		$this->_cPtr=$r;
	}

	function connected() {
		return eslConnection_connected($this->_cPtr);
	}

	function getInfo() {
		$r=eslConnection_getInfo($this->_cPtr);
		return is_resource($r) ? new eslEvent($r) : $r;
	}

	function send($cmd) {
		return eslConnection_send($this->_cPtr,$cmd);
	}

	function sendRecv($cmd) {
		$r=eslConnection_sendRecv($this->_cPtr,$cmd);
		return is_resource($r) ? new eslEvent($r) : $r;
	}

	function sendEvent($send_me) {
		return eslConnection_sendEvent($this->_cPtr,$send_me);
	}

	function recvEvent() {
		$r=eslConnection_recvEvent($this->_cPtr);
		return is_resource($r) ? new eslEvent($r) : $r;
	}

	function recvEventTimed($ms) {
		$r=eslConnection_recvEventTimed($this->_cPtr,$ms);
		return is_resource($r) ? new eslEvent($r) : $r;
	}

	function filter($header,$value) {
		return eslConnection_filter($this->_cPtr,$header,$value);
	}

	function events($etype,$value) {
		return eslConnection_events($this->_cPtr,$etype,$value);
	}

	function execute($app,$arg=null,$uuid=null) {
		switch (func_num_args()) {
		case 1: $r=eslConnection_execute($this->_cPtr,$app); break;
		case 2: $r=eslConnection_execute($this->_cPtr,$app,$arg); break;
		default: $r=eslConnection_execute($this->_cPtr,$app,$arg,$uuid);
		}
		return $r;
	}

	function setBlockingExecute($val) {
		return eslConnection_setBlockingExecute($this->_cPtr,$val);
	}

	function setEventLock($val) {
		return eslConnection_setEventLock($this->_cPtr,$val);
	}
}


?>