<?php
/*!
* Pi Dashboard (http://www.nxez.com)
* Copyright 2017 NXEZ.com.
* Licensed under the GPL v3.0 license.
*/

@header("content-Type: text/html; charset=utf-8");
ob_start();
date_default_timezone_set('Europe/Berlin');

$D = Array();
$D['page']['time']['start'] = explode(' ', microtime());
get_info();
$data = array();

if (isset($_GET['ajax']) && $_GET['ajax'] == "true"){

    $key = ftok("/home/pi/projects/", 2);  
    $size = 2048;  
    $shmid = @shmop_open($key, 'c', 0777, $size);  
    if($shmid == FALSE)$D['shm_open'] = 1;
    else $D['shm_open'] = 2;

    // 读取共享内存中的数据
    $shm_data = shmop_read($shmid, 0, 2048);
    if($shm_data == FALSE)$D['shm_read'] = 1;
    else $D['shm_read'] = 2;
    // 对读取的数据进行反序列化
    // if(!empty($shm_data)) $D['shm_data'] = (int)($shm_data[0].$shm_data[1].$shm_data[2].$shm_data[3])/100;
    if(!empty($shm_data)){

        //ADS1115
        $D['shm_data']['VCC'] = (int)($shm_data[0].$shm_data[1].$shm_data[2].$shm_data[3])/100;//VCC
	$D['shm_data']['MIC'] = (int)($shm_data[4].$shm_data[5].$shm_data[6].$shm_data[7]);//MIC
	$D['shm_data']['EZ0_1'] = (int)($shm_data[8].$shm_data[9].$shm_data[10].$shm_data[11]);//EZ0_1
        $D['shm_data']['EZ0_2'] = (int)($shm_data[12].$shm_data[13].$shm_data[14].$shm_data[15]);//EZ0_2

        //TOF
        $D['shm_data']['TOF1'] = (int)($shm_data[16].$shm_data[17].$shm_data[18].$shm_data[19]);//TOF1
        $D['shm_data']['TOF2'] = (int)($shm_data[20].$shm_data[21].$shm_data[22].$shm_data[23]);//TOF2

        //THERMOL
        for($i = 0; $i < 16; $i++){
            $D['shm_data']['THERMOL1'][$i] = (int)($shm_data[24+$i*4].$shm_data[25+$i*4].$shm_data[26+$i*4].$shm_data[27+$i*4])/10;//THERMOL1
            $D['shm_data']['THERMOL2'][$i] = (int)($shm_data[88+$i*4].$shm_data[89+$i*4].$shm_data[90+$i*4].$shm_data[91+$i*4])/10;//THERMOL2
        }

        //RPLidar_Zahl
        $D['shm_data']['RPLidar_Zahl'] = (int)($shm_data[152].$shm_data[153].$shm_data[154].$shm_data[155]);//RPLidar_Zahl
        for($i = 0; $i < $D['shm_data']['RPLidar_Zahl']; ++$i)$D['shm_data']['RPLidar_Abstand'][$i]=(int)($shm_data[156+$i*4].$shm_data[157+$i*4].$shm_data[158+$i*4].$shm_data[159+$i*4]);//RPLidar_Abstand



    }

    shmop_close($shmid);

    echo json_encode($D);
    exit;
}

$D['version'] = '1.1.0';
$D['model'] = get_device_model();
$D['user'] = @get_current_user();
$D['hostname'] = gethostname();
$D['hostip'] = ('/'==DIRECTORY_SEPARATOR) ? $_SERVER['SERVER_ADDR'] : @gethostbyname($_SERVER['SERVER_NAME']);
$D['yourip'] = $_SERVER['REMOTE_ADDR'];
$D['uname'] = @php_uname();
$D['os'] = explode(" ", php_uname());

if (($str = @file("/proc/cpuinfo")) !== false){
    $str = implode("", $str);
    @preg_match_all("/model\s+name\s{0,}\:+\s{0,}([\w\s\)\(\@.-]+)([\r\n]+)/s", $str, $model);
    @preg_match_all("/BogoMIPS\s{0,}\:+\s{0,}([\d\.]+)[\r\n]+/", $str, $bogomips);
    @preg_match_all("/Model\s{0,}\:+\s{0,}([\w\s\)\(\@.-]+)([\r\n]+)/s", $str, $pimodel);

    if (false !== is_array($model[1])){
        $D['cpu']['count'] = sizeof($model[1]);
        $bogomips[1][0] = ' | Bogomips:'.$bogomips[1][0];
        if($D['cpu']['count'] == 1) $D['cpu']['model'] = $model[1][0].$bogomips[1][0];
        else $D['cpu']['model'] = $model[1][0].$bogomips[1][0].' ×'.$D['cpu']['count'];
    }

    if (false !== is_array($pimodel[1]))$D['model']['pimodel'] = $pimodel[1][0];
}
else{
    $D['cpu']['count'] = 1;
    $D['cpu']['model'] = '';
    $D['model']['pimodel'] = '';
}

function get_device_model(){
    return ['name' => 'Raspberry Pi', 'id' => 'raspberry-pi'];
}

function get_info(){
    global $D;

    $D['time'] = time();

    if (($str = @file("/proc/uptime")) !== false){
        $str = explode(" ", implode("", $str));
        $D['uptime'] = trim($str[0]);
    }
    else $D['uptime'] = 0;

    if (($str = @file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq")) !== false) $D['cpu']['freq'] = $str[0];
    else $D['cpu']['freq'] = 0;

    // CPU Core
    if (($str = @file("/proc/stat")) !== false){
        $str = str_replace("  ", " ", $str);
        $info = explode(" ", implode("", $str));
        $D['cpu']['stat'] = array('user'=>$info[1],
            'nice'=>$info[2],
            'sys' => $info[3],
            'idle'=>$info[4],
            'iowait'=>$info[5],
            'irq' => $info[6],
            'softirq' => $info[7]
        );
    }
    else{
        $D['cpu']['stat'] = array('user'=>0,
            'nice'=>0,
            'sys' => 0,
            'idle'=> 0,
            'iowait'=> 0,
            'irq' => 0,
            'softirq' => 0
        );
    }


    if (($str = @file("/sys/class/thermal/thermal_zone0/temp")) !== false) $D['cpu']['temp'] = $str;
    else $D['cpu']['temp'] = 0;


    if (($str = @file("/proc/meminfo")) !== false){
        $str = implode("", $str);

        preg_match_all("/MemTotal\s{0,}\:+\s{0,}([\d\.]+).+?MemFree\s{0,}\:+\s{0,}([\d\.]+).+?Cached\s{0,}\:+\s{0,}([\d\.]+).+?SwapTotal\s{0,}\:+\s{0,}([\d\.]+).+?SwapFree\s{0,}\:+\s{0,}([\d\.]+)/s", $str, $buf);
        preg_match_all("/Buffers\s{0,}\:+\s{0,}([\d\.]+)/s", $str, $buffers);

        $D['mem']['total'] = round($buf[1][0]/1024, 2);
        $D['mem']['free'] = round($buf[2][0]/1024, 2);
        $D['mem']['cached'] = round($buf[3][0]/1024, 2);
        $D['mem']['used'] = $D['mem']['total']-$D['mem']['free'];
        $D['mem']['percent'] = (floatval($D['mem']['total'])!=0)?round($D['mem']['used']/$D['mem']['total']*100,2):0;
        $D['mem']['real']['used'] = $D['mem']['total'] - $D['mem']['free'] - $D['mem']['cached'] - $D['mem']['buffers'];
        $D['mem']['swap']['total'] = round($buf[4][0]/1024, 2);
    }
    else{
        $D['mem']['total'] = 0;
        $D['mem']['free'] = 0;
        $D['mem']['cached'] = 0;
        $D['mem']['used'] = 0;
        $D['mem']['percent'] = 0;
        $D['mem']['real']['used'] = 0;
        $D['mem']['swap']['total'] = 0;
    }

    if (($str = @file("/proc/loadavg")) !== false){
        $str = explode(" ", implode("", $str));
        $str = array_chunk($str, 4);
        $D['load_avg'] = $str[0];
    }
    else $D['load_avg'] = array(0,0,0,'0/0');

    $D['disk']['total'] = round(@disk_total_space(".")/(1024*1024*1024),3);
    $D['disk']['free'] = round(@disk_free_space(".")/(1024*1024*1024),3);
    $D['disk']['used'] = $D['disk']['total'] - $D['disk']['free'];
    $D['disk']['percent'] = (floatval($D['disk']['total'])!=0)?round($D['disk']['used']/$D['disk']['total']*100,2):0;


    if (($strs = @file("/proc/net/dev")) !== false){
        $D['net']['count'] = count($strs) - 2;
        for ($i = 2; $i < count($strs); $i++ )
        {
            preg_match_all( "/([^\s]+):[\s]{0,}(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)/", $strs[$i], $info );
            $D['net']['interfaces'][$i-2]['name'] = $info[1][0];
            $D['net']['interfaces'][$i-2]['total_in'] = $info[2][0];
            $D['net']['interfaces'][$i-2]['total_out'] = $info[10][0];
        }
   	$D['net']['interfaces'][0]['total_in'] = $D['shm_data']['EZ0_1'];
   	$D['net']['interfaces'][0]['total_out'] = $D['shm_data']['EZ0_2'];
   	$D['net']['interfaces'][1]['total_in'] = $D['shm_data']['TOF1'];
    $D['net']['interfaces'][1]['total_out'] = $D['shm_data']['TOF2'];
    }

    

    else{
        $D['net']['count'] = 0;
    }
}
?>