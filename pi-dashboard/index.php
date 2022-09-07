<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'device.php');
?>
<html>

<!--配置网页标签栏信息和设置文本格式-->
<head>
    <meta charset="UTF-8" />
    <title>Daten der Sensoren</title>
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="shortcut icon" href="favicon.ico" type="image/x-icon" />

    <!--导入外部文件/库-->
    <link href="assets/bootstrap.min.css" rel="stylesheet">
    <script src="assets/jquery-3.1.1.min.js"></script> 
    <script src="assets/highcharts.js"></script>
    <script src="assets/highcharts-more.js"></script>
    <script src="assets/solid-gauge.js"></script>
    <script src="assets/exporting.js"></script>
    <script src="assets/bootstrap.min.js"></script>
    <!--导入外部文件/库-->

    <!--声明并定义变量和结构体-->
    <script language="JavaScript">
        window.dashboard_old = null;
        window.dashboard = null;
        var init_vals = eval('('+"{'mem': {'total':<?php echo($D['mem']['total']) ?>,'swap':{'total':<?php echo($D['mem']['swap']['total']) ?>}}, 'disk': {'total':<?php echo($D['disk']['total']) ?>}, 'net': { 'count': <?php echo($D['net']['count']) ?>} }"+')');
    </script>
    <!--声明并定义变量和结构体-->

    <style type="text/css">
	body{margin: 0;}
        .label {color: #999999; font-size: 75%; font-weight: bolder;}
        .tuc {width: 100%; height: 200px; background-image: url("tuc.png"); background-size: 100%;background-repeat: repeat-x; background-position: center;}
        .container-circle {position: relative; width: 100%; height: 400px; background-image: url("Achse.png"); background-size: 100%;background-repeat: repeat-x; background-position: center;overflow: hidden;}
	    .container-44L {position: relative; width: 100%; height: 400px;}
        .dot {position: absolute; width: 5px; height: 5px;}
        .point {position: absolute; width: 2px; height: 2px; background-color: #FFFFFF;}
        .mic {float: top; width: 100%; height: 50%; margin:0; background-color: #CCCCCC;}
        .box44L {position: absolute; width: 100%; height: 50%; background-color: #CCCCCC;}
        .box44L-left {float:left; width: 49.5%; height: 100%;}
        .box1 {width: 100%; height: 25%;}
        .box2 {width: 100%; height: 25%;}
        .box3 {width: 100%; height: 25%;}
        .box4 {width: 100%; height: 25%;}
	    .box44L-right {float:right; width: 49.5%; height: 100%;}
        .box5 {width: 100%; height: 25%;}
        .box6 {width: 100%; height: 25%;}
        .box7 {width: 100%; height: 25%;}
        .box8 {width: 100%; height: 25%;}
        .btn {float:left; width: 25%; height: 100%; background-color: transparent;overflow: hidden;}        
    </style>

</head>
<!--配置网页标签栏信息和设置文本格式-->

<body>
<div id="app">
    <!--配置网页顶的菜单栏-->
    <nav class="navbar navbar-default">
        <div class="container">
            <div class="navbar-header">
                <button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target="#bs-example-navbar-collapse-1">
                    <span class="sr-only">Toggle navigation</span>
                    <span class="icon-bar"></span>
                    <span class="icon-bar"></span>
                    <span class="icon-bar"></span>
                </button>
                <a class="navbar-brand" href="#"><img style="height: 100%; display: inline; margin-right: 10px;" src="assets/logo.png">Daten der Sensoren</a>
            </div>

            <div class="collapse navbar-collapse" id="bs-example-navbar-collapse-1">
                <ul class="nav navbar-nav navbar-right">
                    <li class="dropdown">
                        <a href="#" class="dropdown-toggle" data-toggle="dropdown" role="button" aria-expanded="false">About <span class="caret"></span></a>
                        <ul class="dropdown-menu" role="menu">
                            <li><a target="_blank" href="https://www.tu-chemnitz.de/">Technische Universität Chemnitz</a></li>
                            <li><a target="_blank" href="https://www.tu-chemnitz.de/mb/MHT/">Professur Montage- und Handhabungstechnik</a></li>
                            <li><a target="_blank" href="https://github.com/HUST-IST/RaspberryPi.git">der Verwahrungsort auf GitHub</a></li>
                        </ul>
                    </li>
                </ul>
            </div>
        </div>
    </nav>
    <!--配置网页顶的菜单栏-->


    <div class="container">
        <div class="row">
            <div class="col-md-3">
                <div class="tuc" style="text-align: center; padding: 20px 0;"></div>
                <div style="background-color: #E0E0E0; padding: 5px; border-radius: 3px;">
                    <div class="text-center" style="margin:10px; padding: 10px 0 10px 0; border-radius: 3px;"><div id="pimodel" style="font-size: 90%; font-weight: bolder; text-shadow: 0 1px 0 #fff;"><?php echo($D['model']['pimodel']); ?></div></div>
                    <div class="text-center" style="margin:20px; padding: 10px 0 10px 0; background-color:#CEFCA3; border-radius: 3px;"><div class="label">IP</div><div id="hostip" style="font-size: 150%; font-weight: bolder;"><?php echo($D['hostip']); ?></div></div>
                    <div class="text-center" style="margin:20px; padding: 10px 0 10px 0; background-color:#9DCFFB; border-radius: 3px;"><div class="label">TIME</div><div id="time" style="font-size: 150%; font-weight: bolder;">00:00</div><div id="date">-</div></div>
                    <div class="text-center" style="margin:20px; padding: 10px 0 10px 0; background-color:#FFFECD; border-radius: 3px;"><div class="label">UPTIME</div><div id="uptime" style="font-size: 120%; font-weight: bolder;">0</div></div>
                    <div class="text-center" style="margin:20px; padding: 10px 0 10px 0; background-color:#FAFAFA; border-radius: 3px;"><div class="label">USER</div><div id="user" style="font-size: 120%; font-weight: bolder;"><?php echo($D['user']); ?></div></div>
                    <div class="text-center" style="margin:20px; padding: 10px 0 10px 0; background-color:#FAFAFA; border-radius: 3px;"><div class="label">OS</div><div id="os" style="font-size: 120%; font-weight: bolder;"><?php echo($D['os'][0]); ?></div></div>
                    <div class="text-center" style="margin:20px; padding: 10px 0 10px 0; background-color:#FAFAFA; border-radius: 3px;"><div class="label">HOSTNAME</div><div id="hostname" style="font-size: 110%; font-weight: bolder;"><?php echo($D['hostname']); ?></div></div>
                    <div class="text-center" style="margin:20px; padding: 10px 0 10px 0; background-color:#FAFAFA; border-radius: 3px;"><div id="uname" style="word-break:break-all; word-wrap:break-word; font-size: 12px; color: #999999; padding: 0 10px;"><?php echo($D['uname']); ?></div></div>
                </div>
            </div>
            <div class="col-md-9">
                <div class="row">
                    <div class="col-md-6 col-sm-6">
                        <div id="container-cpu" style="width: 100%; height: 200px;"></div>
                        <div style="height: 200px;">
                            <div class="row" style="margin: 0; background-color:#E0E0E0;">
                                <div class="text-center" style="padding: 2px 0 2px 0; background-color: #CDFD9F;"><strong><small>CPU</small></strong></div>
                                <div class="col-md-3 col-sm-3 col-xs-3" style="padding: 0;">
                                    <div class="text-center" style="padding: 10px 0 10px 0; background-color:#FFFECD;"><span id="cpu-freq" style="font-weight: bolder;"><?php echo($D['cpu']['freq']/1000) ?></span><br /><small class="label">MHz</small></div>
                                </div>
                                <div class="col-md-3 col-sm-3 col-xs-3" style="padding: 0;">
                                    <div class="text-center" style="padding: 10px 0 10px 0;"><span id="cpu-count"><?php echo($D['cpu']['count']) ?></span><br /><small class="label">CORE</small></div>
                                </div>
                                <div class="col-md-3 col-sm-3 col-xs-3" style="padding: 0;">
                                    <div class="text-center" style="padding: 10px 0 10px 0; background-color:#FDCCCB;"><span id="cpu-temp" style="font-weight: bolder;">0</span><br /><small class="label">C°</small></div>
                                </div>
                                <div class="col-md-3 col-sm-3 col-xs-3" style="padding: 0;">
                                    <div class="text-center" style="padding: 10px 0 10px 0; background-color:#9BCEFD;"><span id="cpu-stat-idl">0</span>%<br /><small class="label">IDLE</small></div>
                                </div>
                                <div class="col-md-2 col-sm-2 col-xs-2" style="padding: 0;">
                                    <div class="text-center" style="padding: 10px 0 10px 0; background-color:#9BCEFD;"><span id="cpu-stat-use">0</span>%<br /><small class="label">USER</small></div>
                                </div>
                                <div class="col-md-2 col-sm-2 col-xs-2" style="padding: 0;">
                                    <div class="text-center" style="padding: 10px 0 10px 0; background-color:#9BCEFD;"><span id="cpu-stat-sys">0</span>%<br /><small class="label">SYS</small></div>
                                </div>
                                <div class="col-md-2 col-sm-2 col-xs-2" style="padding: 0;">
                                    <div class="text-center" style="padding: 10px 0 10px 0; background-color:#9BCEFD;"><span id="cpu-stat-nic">0</span>%<br /><small class="label">NICE</small></div>
                                </div>
                                <div class="col-md-2 col-sm-2 col-xs-2" style="padding: 0;">
                                    <div class="text-center" style="padding: 10px 0 10px 0; background-color:#9BCEFD;"><span id="cpu-stat-iow">0</span>%<br /><small class="label">IOW</small></div>
                                </div>
                                <div class="col-md-2 col-sm-2 col-xs-2" style="padding: 0;">
                                    <div class="text-center" style="padding: 10px 0 10px 0; background-color:#9BCEFD;"><span id="cpu-stat-irq">0</span>%<br /><small class="label">IRQ</small></div>
                                </div>
                                <div class="col-md-2 col-sm-2 col-xs-2" style="padding: 0;">
                                    <div class="text-center" style="padding: 10px 0 10px 0; background-color:#9BCEFD;"><span id="cpu-stat-sirq">0</span>%<br /><small class="label">SIRQ</small></div>
                                </div>
                                <div class="col-md-12" style="min-height: 52px;">
                                    <div class="text-center" style="padding: 10px 0 10px 0; margin: auto 0;"><span id="cpu-model" class="label"><?php echo($D['cpu']['model']) ?></span>&nbsp;</div>
                                </div>
                            </div>
                        </div>
                    </div>
                    <div class="col-md-6 col-sm-6">
                        <div id="container-mem" style="width: 100%; height: 200px;"></div>
                        <div style="height: 200px;">
                            <div class="row" style="margin: 0; background-color:#E0E0E0;">
                                <div class="text-center" style="padding: 2px 0 2px 0; background-color: #CDFD9F;"><strong><small>MEMORY</small></strong></div>
                                <div class="col-md-6 col-sm-6 col-xs-6" style="padding: 0;">
                                    <div class="text-center" style="padding: 10px 0 10px 0;"><span id="mem-percent">0</span>%<br /><small class="label">USED</small></div>
                                    <div class="text-center" style="padding: 10px 0 10px 0; background-color: #CDFD9F;"><span id="mem-free">0</span>MB<br /><small class="label">FREE</small></div>
                                </div>
                                <div class="col-md-6 col-sm-6 col-xs-6" style="padding: 0;">
                                    <div class="text-center" style="padding: 10px 0 10px 0; background-color: #CEFFFF;"><span id="mem-cached">0</span>MB<br /><small class="label">CACHED</small></div>
                                    <div class="text-center" style="padding: 10px 0 10px 0; background-color: #CCCDFC;"><span id="mem-swap-total">0</span>MB<br /><small class="label">SWAP</small></div>
                                </div>
                                <div class="col-md-3 col-sm-3 col-xs-3" style="padding: 0;">
                                    <div class="text-center" style="padding: 10px 0 10px 0;"><span id="loadavg-1m">0.0</span><br /><small class="label">AVG.1M</small></div>
                                </div>
                                <div class="col-md-3 col-sm-3 col-xs-3" style="padding: 0;">
                                    <div class="text-center" style="padding: 10px 0 10px 0;"><span id="loadavg-5m">0.0</span><br /><small class="label">AVG.5M</small></div>
                                </div>
                                <div class="col-md-3 col-sm-3 col-xs-3" style="padding: 0;">
                                    <div class="text-center" style="padding: 10px 0 10px 0;"><span id="loadavg-10m">0.0</span><br /><small class="label">AVG.10M</small></div>
                                </div>
                                <div class="col-md-3 col-sm-3 col-xs-3" style="padding: 0;">
                                    <div class="text-center" style="padding: 10px 0 10px 0; background-color: #FFFDCF;"><strong><span id="loadavg-running">0</span>/<span id="loadavg-threads">0</span></strong><br /><small class="label">RUNNING</small></div>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>


                <div class="row">
                    <div class="col-md-12">
                        <div class="row" style="margin: 0;">
                            <div class="col-md-10 col-sm-10 col-xs-10" style="padding: 0;">
                                <div id="container-net-interface-<?php echo(3) ?>" style="min-width: 100%; height: 150px; margin: 20 auto"></div>
                            </div>
                            <div class="col-md-2 col-sm-2 col-xs-2" style="padding: 0;">
                                <div style="height: 80px; margin-top: 10px;">
                                    <div class="text-center" style="padding: 2px 0 2px 0; background-color: #CCCCCC;"><strong>Netz</strong></div>
                                    <div class="text-center" style="padding: 10px 0 10px 0; background-color: #9BCEFD;"><span id="net-interface-3-total-in"><?php echo($D['net']['interfaces'][2]['total_in']) ?></span><br /><small class="label">IN</small></div>
                                    <div class="text-center" style="padding: 10px 0 10px 0; background-color: #CDFD9F;"><span id="net-interface-3-total-out"><?php echo($D['net']['interfaces'][2]['total_out']) ?></span><br /><small class="label">OUT</small></div>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
                <div class="row">
                	<div class="col-md-6 col-sm-6" style="margin-top: 10px;">
			            <div class="container-circle"><div class="dot"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div><div class="point"></div></div>
            		</div>
			        <div class="col-md-6 col-sm-6">
				        <div class="container-44L" style="margin-top: 10px;">
                            <div class="mic"></div>
                            <div class="box44L">
                                <div class="box44L-left">
                                    <div class="box1">
                                        <div class="btn btn1"></div><div class="btn btn2"></div><div class="btn btn3"></div><div class="btn btn4"></div>
                                    </div>
                                    <div class="box2">
                                        <div class="btn btn5"></div><div class="btn btn6"></div><div class="btn btn7"></div><div class="btn btn8"></div>
                                    </div>
                                    <div class="box3">
                                        <div class="btn btn9"></div><div class="btn btn10"></div><div class="btn btn11"></div><div class="btn btn12"></div>
                                    </div>
                                    <div class="box4">
                                        <div class="btn btn13"></div><div class="btn btn14"></div><div class="btn btn15"></div><div class="btn btn16"></div>
                                    </div>
                                </div>
				                <div class="box44L-right">
                                    <div class="box5">
                                        <div class="btn btn17"></div><div class="btn btn18"></div><div class="btn btn19"></div><div class="btn btn20"></div>
                                    </div>
                                    <div class="box6">
                                        <div class="btn btn21"></div><div class="btn btn22"></div><div class="btn btn23"></div><div class="btn btn24"></div>
                                    </div>
                                    <div class="box7">
                                        <div class="btn btn25"></div><div class="btn btn26"></div><div class="btn btn27"></div><div class="btn btn28"></div>
                                    </div>
                                    <div class="box8">
                                        <div class="btn btn29"></div><div class="btn btn30"></div><div class="btn btn31"></div><div class="btn btn32"></div>
                                    </div>
                                </div>
                            </div>
                        </div>
			        </div>
        	    </div>
		
		        <div class="row">
                    <div class="col-md-12">
			            <div class="row" style="margin: 0;">
                             <div class="col-md-10 col-sm-10 col-xs-10" style="padding: 0;">
                                  <div id="container-net-interface-<?php echo(2) ?>" style="min-width: 100%; height: 150px; margin: 20 auto"></div>
                             </div>
                             <div class="col-md-2 col-sm-2 col-xs-2" style="padding: 0;">
                                 <div style="height: 80px; margin-top: 10px;">
                                    <div class="text-center" style="padding: 2px 0 2px 0; background-color: #CCCCCC;"><strong>ToF</strong></div>
                                    <div class="text-center" style="padding: 10px 0 10px 0; background-color: #9BCEFD;"><span id="net-interface-2-total-in"><?php echo($D['net']['interfaces'][1]['total_in']) ?></span><br /><small class="label">Sensor1</small></div>
                                    <div class="text-center" style="padding: 10px 0 10px 0; background-color: #CDFD9F;"><span id="net-interface-2-total-out"><?php echo($D['net']['interfaces'][1]['total_out']) ?></span><br /><small class="label">Sensor2</small></div>
                                </div>
                            </div>
                        </div>
			            <div class="row" style="margin: 0;">
                            <div class="col-md-10 col-sm-10 col-xs-10" style="padding: 0;">
                                <div id="container-net-interface-<?php echo(1) ?>" style="min-width: 100%; height: 150px; margin: 20 auto"></div>
                            </div>
                            <div class="col-md-2 col-sm-2 col-xs-2" style="padding: 0;">
                                <div style="height: 80px; margin-top: 10px;">
                                    <div class="text-center" style="padding: 2px 0 2px 0; background-color: #CCCCCC;"><strong>EZ0</strong></div>
                                    <div class="text-center" style="padding: 10px 0 10px 0; background-color: #9BCEFD;"><span id="net-interface-1-total-in"><?php echo($D['net']['interfaces'][0]['total_in']) ?></span><br /><small class="label">Sensor1</small></div>
                                    <div class="text-center" style="padding: 10px 0 10px 0; background-color: #CDFD9F;"><span id="net-interface-1-total-out"><?php echo($D['net']['interfaces'][0]['total_out']) ?></span><br /><small class="label">Sensor2</small></div>
                                </div>
                            </div>
                        </div>
		            </div>
                </div>
        </div>
    </div>
    <div class="container">
        <div class="row">
            <div class="col-md-12">
                <div id="footer">
                    <hr style="margin: 20px 0 10px 0;" />
                    <p class="pull-left" style="font-size: 12px;">Powered by <a target="_blank" href="https://make.quwj.com/project/10"><strong>Pi Dashboard</strong></a> v<?php echo($D['version']) ?>, <a target="_blank" href="https://www.nxez.com">NXEZ.com</a> all rights reserved.
                    </p>
                </div>
            </div>
        </div>
    </div>
</div>
<script src="assets/dashboard.min.js"></script>
</body>
</html>