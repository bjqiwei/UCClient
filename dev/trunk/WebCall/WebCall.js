(function () {
    
    window.WebCall = window.WebCall || {
            _thisPath: "",

            callid: null,

            SessionS: {},
            userAgent: null,
            loglevel:"debug",
            websocketurl: null,
            websocket:null,
            STATUS:{
            // call states
            STATUS_NULL: 0,
            STATUS_CONSULTATIONING: 1,
            STATUS_RECONNECTING:2,
            STATUS_CONNECTED: 3,
            STATUS_ALTERNATEING:4,
            STATUS_CONFERENCEING:5,
            STATUS_SINGLESTEPCONFERENCEING:6
            },
            
            Cause:{
                Alternate:0,
                CallCancelled:1,
                CallNotAnswered:2,
                Consultation:3,
                MakeCall:4,
                NewCall:5,
                NormalClearing:6,
                SingleStepConference:7,
                Conference:8,
                SingleStepTransfer:9,
                Transfer:10
            },
            /**
            *设置日志级别
            * @param level:error,warn,info,debug
            **/
            setLogLevel: function (level)
            {
                WebCall.debug("setLogLevel:" + level);
                if(level == "error" || level == "warn" || level == "info" || level == "debug"){
                    WebCall.loglevel = level;
                    //SIP.setDebugLevel(WebCall.loglevel);
                    var cmd = {};
                    cmd.type = "cmd";
                    cmd.cmd = "setTraceFlag";
                    cmd.param = {};
                    cmd.param.enable = true;
                    cmd.param.logFileName = "D:\\sdk.log";
                    cmd.param.level = 99;
                    WebCall.doSend(cmd);
                }
                else {
                    WebCall.error("set log level error, not recognize:"+level);
                }
            },
            
			/**
             * 初始化
             * @param websocketurl
             */
			 
            Connect:function(websocketurl){
                if (typeof (websocketurl) == "undefined") {
                    WebCall.error("websocketurl not init");
                    return false;
                }

                WebCall.websocketurl = websocketurl;
                WebCall.debug("Connect websocketurl:" + WebCall.websocketurl);
                WebCall.websocket = new window.WebSocket(WebCall.websocketurl);

                WebCall.websocket.onopen = WebCall.wsonOpen;
                WebCall.websocket.onclose = WebCall.wsonClose;
                WebCall.websocket.onmessage = WebCall.wsonMessage;
                WebCall.websocket.onerror = WebCall.wsonError;

            },
			 
            /**
             * 初始化
             */
            initialize: function () {
                var cmd = {};
                cmd.type = "cmd";
                cmd.cmd = "initialize";
                //cmd.param = {};
                WebCall.doSend(cmd);
				return true;
            },
            
            wsonOpen:function(evt) { 
                WebCall.debug("WebSocket CONNECTED");
                if (typeof (WebCall.onWSOpen) == "function") {
                    WebCall.onWSOpen();
                }
            },
 
            wsonClose:function(evt) { 
                WebCall.info("WebSocket DISCONNECTED");
                if (typeof (WebCall.onWSClose) == "function") {
                    WebCall.onWSClose(evt);
                }
            },
 
            wsonMessage:function(evt) { 

                var event = JSON.parse(evt.data);
                if (event.type == "cmdresult")
                {
                    switch (event.cmdresult) {
                        case "initialize":
                            {
                                WebCall.debug("Init:" + event.param.return);
                                if (typeof (WebCall.onInitialize) == "function") {
                                    WebCall.onInitialize(event.param.return);
                                }
                                WebCall.getVersion();
                            }
                            return;
                        case "getVersion":
                            {
                                WebCall.info("WebCall version=" + event.param.return);
                                if (typeof (WebCall.onGetVersion) == "function") {
                                    WebCall.onGetVersion(event.param.return);
                                }
                            }
                            return;
                        case "getMicroPhoneInfo":
                            {
                                WebCall.info("WebCall micro info=" + JSON.stringify(event.param.return));
                                if (typeof (WebCall.onGetMicroPhoneInfo) == "function") {
                                    WebCall.onGetMicroPhoneInfo(event.param.return);
                                }
                            }
                            return;
                        case "selectMicroPhone":
                            {
                                WebCall.info("WebCall selectMicroPhone=" + event.param.return);
                                if (typeof (WebCall.onSelectMicroPhone) == "function") {
                                    WebCall.onSelectMicroPhone(event.param.return);
                                }
                            }
                            return;
                        case "setTraceFlag":
                            {
                                WebCall.info("WebCall setTraceFlag=" + event.param.return);
                                if (typeof (WebCall.onSetTraceFlag) == "function") {
                                    WebCall.onSetTraceFlag(event.param.return);
                                }
                            }
                            return;
                        case "connectToCCPServer":
                            {
                                WebCall.info("WebCall connectToCCPServer=" + event.param.return);
                                if (typeof (WebCall.onConnectToCCPServer) == "function") {
                                    WebCall.onConnectToCCPServer(event.param.return);
                                }
                            }
                            return;
                        default:
                            WebCall.warn("WebCall:cmdresult未知命令:" + evt.data);
                            return;
                    }
                }
                else if(event.type == "event")
                {
                    switch(event.event)
                    {
                        case "onLogInfo":
                            {
                                WebCall.trace(evt.data);
                            }
                            return;
                        case "onSipConnect":
                            {
                                if (typeof (WebCall.onSipConnect) == "function") {
                                    WebCall.onSipConnect(event.param);
                                }
                            }
                            break;
                        default:
                            WebCall.warn("WebCall:event未知事件:" + evt.data);
                            return;
                    }
                }
                WebCall.warn("WebCall 未知消息：" + evt.data);
            },  
 
            wsonError:function(evt) { 
                WebCall.error(evt.data);
                if (typeof (WebCall.onWSError) == "function") {
                    WebCall.onWSError(evt);
                }
            },  
 
            doSend: function (cmd) {
                var message = JSON.stringify(cmd)
                WebCall.debug("SENT: " + message);  
                WebCall.websocket.send(message); 
            },
			
            /**
             * 注册
             * @param txtRealm
             * @param sipid
             * @param sipurl
             * @param pwd
             * @param txtDisplayName
             * @param txtWebsocketUrl
             * @param txtICEServer
             */
            Login: function (server, port, sipid, pwd) {
                // catch exception for IE (DOM not ready)
                WebCall.debug("Login,server:" + server + ":" + port + ",SipID:" + sipid  + ",Pwd:" + pwd);
                
                var cmd = {};
                cmd.type = "cmd";
                cmd.cmd = "connectToCCPServer";
                cmd.param = {};
                cmd.param.serverAddr = server;
                cmd.param.serverPort = parseInt(port);
                cmd.param.voipId = sipid;
                cmd.param.voipPwd = pwd;
                WebCall.doSend(cmd);
            },
            // 注销
            Logout: function () {
                WebCall.debug("Logout:");
                if (WebCall.userAgent) {
                    WebCall.userAgent.stop(); // shutdown all sessions
                }
                return 0;
            },
            /**
             * 获取版本号
             * @returns {string}
             */
            getVersion: function () {
                var cmd = {};
                cmd.type = "cmd";
                cmd.cmd = "getVersion";
                //cmd.param = {};
                WebCall.doSend(cmd);
            },
            
			onInfo:function(request) {
				var call_id = request.call_id;
				WebCall.debug('info:' + call_id);
				var msg={callid:call_id};
				if(WebCall.SessionS[call_id]._status === WebCall.STATUS.STATUS_SINGLESTEPCONFERENCEING){
                    WebCall.SessionS[call_id]._status = WebCall.STATUS.STATUS_CONNECTED;
                    msg.cause = WebCall.Cause.SingleStepConference;
					var status_code = parseInt(request.body.substr(0,request.body.indexOf(':')));
					
					if(status_code === 200){
						WebCall.debug("onConferenced:"+JSON.stringify(msg));
						if (typeof(WebCall.onConferenced) == "function") {
							WebCall.onConferenced(msg);
						}
						WebCall.debug('单步会议中');
					}
					else{
						msg.reason = status_code;
						msg.msg = request.body.substr(request.body.indexOf(':')+1);
						WebCall.debug("onConferenceFailed:"+JSON.stringify(msg));
						if (typeof(WebCall.onConferenceFailed) == "function") {
							WebCall.onConferenceFailed(msg);
						}
						WebCall.debug('单步会议失败:' + msg.reason);
					}
				    return;
                }
			},

            //绑定事件
            bindEvent: function(session){
                session.on('progress', function (response,cause) {
                    //debugger;
                    WebCall.debug('progress');
                    var msg = {callid: response.call_id,msg:cause};
                    WebCall.callid = response.call_id;
                    if(WebCall.SessionS[response.call_id]._status === WebCall.STATUS.STATUS_CONSULTATIONING){
                        msg.cause = WebCall.Cause.Consultation;
                    }
                    if(response.status_code == 100){
                        WebCall.debug("onOriginated:" + JSON.stringify(msg));
                        WebCall.info("外呼中...");
                        if (typeof(WebCall.onOriginated) == "function") {
                            WebCall.onOriginated(msg);
                        }
                    }
                    else if(response.status_code == 180 || response.status_code == 183){
                        
						if(response.status_code == 183){
							WebCall.setupRemoteMedia(WebCall.SessionS[WebCall.callid]);
						}
						
                        WebCall.debug("onDelivered:" + JSON.stringify(msg));
                        WebCall.debug('远端振铃...');
                        if (typeof(WebCall.onDelivered) == "function") {
                            WebCall.onDelivered(msg);
                        }
                    }
                        
                });
                    
                session.on('accepted', function (data, cause) {
                    WebCall.debug('accepted:');
                    WebCall.callid = data.call_id || this.id.substr(0,this.id.indexOf(this.from_tag));
                    var msg = {"callid": WebCall.callid,"msg":cause};
                    if(WebCall.SessionS[WebCall.callid]._status === WebCall.STATUS.STATUS_CONSULTATIONING){
                        msg.cause = WebCall.Cause.Consultation;
                    }

					WebCall.setupRemoteMedia(WebCall.SessionS[WebCall.callid]);

                    WebCall.debug("onEstablished:" + JSON.stringify(msg));
                    WebCall.info("通话中");
                    if (typeof(WebCall.onEstablished) == "function") {
                        WebCall.onEstablished(msg);
                    }
                        
                });
                    
                session.on('rejected', function (response, cause) {
                    WebCall.debug('rejected:' + cause);
                });
                
                session.on('failed', function (response, cause) {
                    WebCall.debug('failed:'+cause);
                });
                    
                session.on('terminated', function(message, cause) {
                    WebCall.debug('terminated:' + cause);
                    
                    var call_id = session.id.substr(0,session.id.indexOf(session.from_tag));
                    if(!WebCall.SessionS[call_id]){
                        return;
                    }
                    WebCall.callid = call_id;
                    message = message || {};
                    var msg = {"callid":call_id,"reason":message.status_code ? message.status_code:200,"msg":cause?cause:"OK"};

                    if(WebCall.SessionS[WebCall.callid]._status === WebCall.STATUS.STATUS_CONSULTATIONING){
                        msg.cause = WebCall.Cause.Consultation;
                    }

                    WebCall.debug("onCallCleared:" + JSON.stringify(msg));
                    WebCall.info("通话已挂断:" + msg.reason);
                    if(typeof(WebCall.onCallCleared) == "function"){
                        WebCall.onCallCleared(msg);
                    }

                    WebCall.cleanupMedia(WebCall.SessionS[call_id]);
                    WebCall.SessionS[call_id]._audio && document.body.removeChild(WebCall.SessionS[call_id]._audio);
                    delete WebCall.SessionS[call_id];
                });
                    
                session.on('cancel', function() {
                    WebCall.debug('cancel');
                });
                
                session.on('reinvite', function(session) {
                    WebCall.debug('reinvite');
                });

                session.on('hold', function (session,cause){
                    WebCall.debug('hold');
                });
                    
                session.on('unhold',function(session,cause){
                    WebCall.debug('unhold');
                });

                session.on('reinviteAccepted',function(session){
                    var call_id = session.id.substr(0,session.id.indexOf(session.from_tag));
                    var msg={callid:call_id};
                    WebCall.callid = call_id;
                    WebCall.debug('reinviteAccepted:' + JSON.stringify(msg));
					                        
                    if(WebCall.SessionS[call_id]._status === WebCall.STATUS.STATUS_CONFERENCEING){
						WebCall.SessionS[call_id]._status = WebCall.STATUS.STATUS_CONNECTED;
                        msg.cause = WebCall.Cause.Conference;
						 WebCall.debug("onConferenced:"+JSON.stringify(msg));
                        if (typeof(WebCall.onConferenced) == "function") {
                            WebCall.onConferenced(msg);
                        }
                        WebCall.debug('咨询会议中');
						return;
                    }

                    /*
					if(WebCall.SessionS[call_id]._status === WebCall.STATUS.STATUS_SINGLESTEPCONFERENCEING){
                        WebCall.SessionS[call_id]._status = WebCall.STATUS.STATUS_CONNECTED;
                        msg.cause = WebCall.Cause.SingleStepConference;
                        WebCall.debug("onConferenced:"+JSON.stringify(msg));
                        if (typeof(WebCall.onConferenced) == "function") {
                            WebCall.onConferenced(msg);
                        }
                        WebCall.debug('单步会议中');
						return;
                    }
					*/

                    if(this.local_hold === true){
                        
                        if(WebCall.SessionS[call_id]._status === WebCall.STATUS.STATUS_CONSULTATIONING){
                            var result = WebCall.MakeCall(WebCall.SessionS[call_id]._consultNumber,WebCall.SessionS[call_id]._userdata);
                            msg.newCall = result.callid;
                            msg.cause = WebCall.Cause.Consultation;
                            WebCall.SessionS[msg.newCall]._status = WebCall.STATUS.STATUS_CONSULTATIONING;
                            WebCall.SessionS[call_id]._newCall = result.callid;
                        }
                        
                        if(WebCall.SessionS[call_id]._status === WebCall.STATUS.STATUS_ALTERNATEING){
                            msg.cause = WebCall.Cause.Alternate;
                        }

                        WebCall.debug("onHeld:"+JSON.stringify(msg));
                        if (typeof(WebCall.onHeld) == "function") {
                            WebCall.onHeld(msg);
                        }
                        WebCall.debug('通话保持');
                    }
                    else if(this.local_hold === false){
                        
                        if(WebCall.SessionS[call_id]._status === WebCall.STATUS.STATUS_RECONNECTING){
                            WebCall.SessionS[call_id]._status = WebCall.STATUS.STATUS_CONNECTED;
                            msg.cause = WebCall.Cause.Consultation;
                        }
                        
                        if(WebCall.SessionS[call_id]._status === WebCall.STATUS.STATUS_ALTERNATEING){
                            WebCall.SessionS[call_id]._status = WebCall.STATUS.STATUS_CONNECTED;
                            msg.cause = WebCall.Cause.Alternate;
                        }
                        WebCall.debug("onRetrieved:" + JSON.stringify(msg));
                        if (typeof(WebCall.onRetrieved) == "function") {
                            WebCall.onRetrieved(msg);
                        }
                        WebCall.debug('通话恢复');
                    }
                });
                    
                session.on('reinviteFailed',function(session){
                    var call_id = session.id.substr(0,session.id.indexOf(session.from_tag));
                    var msg={callid:call_id, reason:session.status_code || 504, msg:session.reason_phrase || "error"};
                    WebCall.callid = call_id;
                    WebCall.debug('reinviteFailed:' + JSON.stringify(msg));
                    if(this.local_hold === true){
                        WebCall.debug("onHeldFailed:"+JSON.stringify(msg));
                        if (typeof(WebCall.onHeldFailed) == "function") {
                            WebCall.onHeldFailed(msg);
                        }
                        WebCall.error('通话保持失败');
                    }
                    else if(this.local_hold === false){
                        WebCall.debug("onRetrieveFailed:" + JSON.stringify(msg));
                        if (typeof(WebCall.onRetrieveFailed) == "function") {
                            WebCall.onRetrieveFailed(msg);
                        }
                        WebCall.debug('通话恢复失败');
                    }
                });
                    
                session.on('replaced', function (newSession) {
                    WebCall.debug('replaced');
                });
                    
                session.on('dtmf', function(request, dtmf) {
                    WebCall.debug('dtmf');
                    var msg = {"callid": request.call_id,"dtmf":dtmf};
                    WebCall.callid = request.call_id;
                    WebCall.debug("onDtmfReceived:" + JSON.stringify(msg));
                    if (typeof(WebCall.onDtmfReceived) == "function") {
                        WebCall.onDtmfReceived(msg);
                    }
                });
                    
                session.on('bye', function(request) {
                    //debugger;
                    WebCall.debug('bye:' + request.call_id);
                    var msg = {"callid":request.call_id,"reason":200,"msg":request.method};
                    WebCall.callid = request.call_id;

					if(WebCall.SessionS[request.call_id]._status === WebCall.STATUS.STATUS_CONFERENCEING){
                        msg.cause = WebCall.Cause.Conference;
                    }

                    WebCall.debug("onCallCleared:" + JSON.stringify(msg));
                    WebCall.info("通话已挂断:" + msg.reason);
                    
                    if(typeof(WebCall.onCallCleared) == "function"){
                        WebCall.onCallCleared(msg);
                    }

                    WebCall.cleanupMedia(WebCall.SessionS[request.call_id]);
                    WebCall.SessionS[request.call_id]._audio && document.body.removeChild(WebCall.SessionS[request.call_id]._audio);
                    delete WebCall.SessionS[request.call_id];
                });
                    
                session.on('referRequested', function(context) {
                    WebCall.debug('referRequested');
                });
                
                session.on('referRequestAccepted', function (referClientContext) {
                    WebCall.debug('referRequestAccepted');
                });
                
                session.on('referRequestRejected', function (referClientContext) {
                    var call_id = referClientContext.call_id;
                    var msg = {callid:call_id,reason:referClientContext.status_code,msg:referClientContext.reason_phrase};
                    WebCall.callid = referClientContext.call_id;
                    WebCall.debug('referRequestRejected:' + JSON.stringify(msg));
                    WebCall.debug("onTransferFailed:" + JSON.stringify(msg));
                    if (typeof(WebCall.onTransferFailed) == "function") {
                        WebCall.onTransferFailed(msg);
                    }
                    WebCall.error('呼叫转接失败');
                });
                
                session.on('referProgress', function(referClientContext) {
                    WebCall.debug('referProgress');
                });
                
                session.on('referAccepted', function (referClientContext) {
                    WebCall.debug('referAccepted');
                });
                
                session.on('referRejected', function (referClientContext) {
                    var call_id = referClientContext.call_id;
                    var msg = {callid:call_id,reason:referClientContext.status_code,msg:referClientContext.reason_phrase};
                    WebCall.callid = call_id;
                    WebCall.debug('referRejected:' + JSON.stringify(msg));
                    WebCall.debug("onTransferFailed:" + JSON.stringify(msg));
                    if (typeof(WebCall.onTransferFailed) == "function") {
                        WebCall.onTransferFailed(msg);
                    }
                    WebCall.error('呼叫转接失败');
                });
                
                session.on('referInviteSent', function (referServerContext) {
                    WebCall.debug('referInviteSent');
                });
                
                session.on('notify', function (request) {
                    var msg = {"callid": request.call_id};
                    WebCall.callid = request.call_id;
                    WebCall.debug('notify:' + JSON.stringify(msg));
                    WebCall.debug("onTransferred:" + JSON.stringify(msg));
                    if (typeof(WebCall.onTransferred) == "function") {
                        WebCall.onTransferred(msg);
                        }
                    WebCall.info('呼叫转接结束');
                });
                
                session.on('refer', function(context) {
                    var msg = {"callid": context.call_id};
                    WebCall.callid = context.call_id;
                    WebCall.debug('refer:' + JSON.stringify(msg));
                });
            },
            
            //呼叫
            MakeCall: function (called,userdata) {
                WebCall.debug("MakeCall:" + called);
                var session = null;
                if (WebCall.userAgent) {
                    // create call session
                    session = WebCall.userAgent.invite(called, {sessionDescriptionHandlerOptions: {
                        constraints: {
                            audio:{deviceId:WebCall.microId},
                            video: false
						}
                    },
					extraHeaders:[
						"P-User-to-User:" + (userdata ? typeof(userdata) === "string" ? userdata: JSON.stringify(userdata): "")
					],
					onInfo:WebCall.onInfo,
					});
                    
                    if (session == null) {
                        
                        WebCall.error('Failed to make call');
                        if (typeof(WebCall.onMakeCallFailed) == "function") {
                            var msg = {};
                            msg.reason = 1;
                            msg.msg="make call failed";
                            WebCall.onMakeCallFailed(msg);
                            WebCall.debug("onMakeCallFailed:" + JSON.stringify(msg));
                            return {result:1};
                        }
                    }
                    
                    var call_id = session.id.substr(0,session.id.indexOf(session.from_tag));
                    WebCall.debug("MakeCall:" + call_id);
                    WebCall.bindEvent(session);

                    WebCall.SessionS[call_id] = session;
                    WebCall.callid = call_id;
                    return {result:0,callid:call_id};
                }
                else if (typeof(WebCall.onMakeCallFailed) == "function") {
                    var msg = {};
                    msg.reason = 1;
                    msg.msg="未注册";
                    WebCall.onMakeCallFailed(msg);
                    WebCall.debug("onMakeCallFailed:" + JSON.stringify(msg));
                    return {result:1};
                }
                return {result:2};
            },
            //摘机
            AnswerCall: function (callid, userdata) {
                WebCall.debug("AnswerCall:" + callid);
                if (callid && WebCall.SessionS[callid]) {
                    WebCall.debug('Connecting...');
                    WebCall.SessionS[callid].accept({extraHeaders:[
						"P-User-to-User:" + (userdata ? typeof(userdata) === "string" ? userdata: JSON.stringify(userdata): "")
					],
					onInfo:WebCall.onInfo
					});
                }
            },
            // 挂断 (SIP BYE or CANCEL)
            ClearCall: function (callid, reason) {
                WebCall.debug("ClearCall, callid:" + callid + ",reason:" + reason);
                if (callid) {
                    WebCall.debug('Terminating the call...'+callid);
                    
                    try{
                        WebCall.SessionS[callid].terminate({status_code:reason});
                    }
                    catch(e) {
                        delete WebCall.SessionS[callid];
                    };
                }
                else{
                    if(callid === null)
                        return;

                    WebCall.debug('Terminating all call...');
                    var call_id;
                    for(call_id in WebCall.SessionS){
                        WebCall.debug('Terminating the call...'+call_id);
                        try {
                            WebCall.SessionS[call_id].terminate({status_code:reason});
                        }
                        catch(e) {
                            delete WebCall.SessionS[call_id];
                        };
                    }
                    
                }
                return 0;
            },
            //发送DTMF
            SendDTMF: function (callid, c) {
                var err = 1;
                if (callid && WebCall.SessionS[callid] && c) {
                    err = WebCall.SessionS[callid].dtmf(c);
                }
                WebCall.debug("SendDTMF,callid:" + callid + ",c:" + c + ",result:" + err);
            },
            // 盲转
            SingleStepTransferCall: function (callid, s_destination, userdata) {
                WebCall.debug("SingleStepTransferCall,callid:" + callid+ ",destination:" + s_destination);
                if (callid && WebCall.SessionS[callid]) {
                    WebCall.debug('SingleStepTransfering the call...'+callid);                    
                    var session = WebCall.SessionS[callid].refer(s_destination,{extraHeaders:[
						"P-User-to-User:" + (userdata ? typeof(userdata) === "string" ? userdata: JSON.stringify(userdata): "")
					]});
                    return 0;
    
                }
                else{
                    WebCall.error("SingleStepTransferCall, the call is not exist.");
                    return 1;
                }
            },
			
			// 咨询后转接
            TransferCall: function (heldCall, transferTargetCall) {
                WebCall.debug("TransferCall,heldCall:" + heldCall+ ",transferTargetCall:" + transferTargetCall);
                if (heldCall && WebCall.SessionS[heldCall] && transferTargetCall && WebCall.SessionS[transferTargetCall]) {
                    WebCall.debug('Transfering the call...');                    
                    var session = WebCall.SessionS[heldCall].refer(WebCall.SessionS[transferTargetCall]);
                    return 0;
    
                }
                else{
                    WebCall.error("TransferCall, the call is not exist.");
                    return 1;
                }
            },
            /**
             * 保持
             * @param callid
             * @returns {number}
             * @constructor
             */
            HoldCall: function (callid) {
                WebCall.debug("HoldCall,callid:" + callid);
                if (callid && WebCall.SessionS[callid]) {
                    WebCall.SessionS[callid].hold();
                    WebCall.debug('Holding the call...'+callid);
                    return 0;
                } else {
                    WebCall.error("HoldCall, the call is not exist.");
                    return 1;
                }
            },
            /**
             * 恢复
             * @param callid
             * @returns {number}
             * @constructor
             */
            RetrieveCall: function (callid) {
                WebCall.debug("RetrieveCall,callid:" + callid);
                if (callid && WebCall.SessionS[callid]) {
                    WebCall.SessionS[callid].unhold();
                    WebCall.debug('Retrieve the call...' + callid);                    
                    return 0;
                  
                } else {
                    WebCall.error("RetrieveCall, the call is not exist.");
                    return 2;
                }
            },
            
            //咨询
            ConsultationCall:function(callid,called, userdata){
                WebCall.debug("ConsultationCall,callid:" + callid + ",called:" + called);
                if (callid && WebCall.SessionS[callid]) {
                    WebCall.SessionS[callid]._status = WebCall.STATUS.STATUS_CONSULTATIONING;
                    WebCall.SessionS[callid]._consultNumber = called;
                    WebCall.SessionS[callid]._userdata = userdata;
                    WebCall.SessionS[callid].hold();
                    WebCall.debug('hold the call...' + callid);
                    //WebCall.MakeCall(called);                    
                    return 0;
                  
                } else {
                    WebCall.error("ConsultationCall, the call is not exist.");
                    return 3;
                }
            },
            
            /**
             * 取消咨询
             * @param callid
             * @constructor
             */
            ReconnectCall: function (activeCall, heldCall) {
                WebCall.debug("ReconnectCall,activeCall:" + activeCall+",heldCall:" + heldCall);
                if (heldCall && WebCall.SessionS[heldCall]) {
                    WebCall.SessionS[heldCall]._status = WebCall.STATUS.STATUS_RECONNECTING;
                    WebCall.debug('ReconnectCall the call...' + heldCall);                    
                    WebCall.SessionS[heldCall].unhold();
                }
                else {
                    WebCall.error("ReconnectCall, the call is not exist.");
                }

                WebCall.ClearCall(activeCall);

            },
            
            //切换通话
            AlternateCall:function (activeCall,otherCall){
                WebCall.debug("AlternateCall,activeCall:" + activeCall+",otherCall:" + otherCall);
                if (!activeCall || !WebCall.SessionS[activeCall]){
                    WebCall.error("AlternateCall, the activeCall is not exist.");
                    return 1;
                }
                
                if (!otherCall || !WebCall.SessionS[otherCall]){
                    WebCall.error("AlternateCall, the otherCall is not exist.");
                    return 1;
                }
                WebCall.SessionS[activeCall]._status = WebCall.STATUS.STATUS_ALTERNATEING;
                WebCall.SessionS[otherCall]._status = WebCall.STATUS.STATUS_ALTERNATEING;
                WebCall.HoldCall(activeCall);
                WebCall.RetrieveCall(otherCall);
            },
			
			//咨询后会议
            ConferenceCall:function (heldCall, otherCall){
                WebCall.debug("ConferenceCall,otherCall:" + otherCall+",heldCall:" + heldCall);
                if (!heldCall || !WebCall.SessionS[heldCall]){
                    WebCall.error("ConferenceCall, the heldCall is not exist.");
                    return 1;
                }
                
                if (!otherCall || !WebCall.SessionS[otherCall]){
                    WebCall.error("ConferenceCall, the otherCall is not exist.");
                    return 1;
                }
				WebCall.SessionS[heldCall]._status = WebCall.STATUS.STATUS_CONFERENCEING;
				WebCall.SessionS[heldCall].reinvite({extraHeaders:["P-Conf-MetaData: type=1;join=true"]});
				
				WebCall.SessionS[otherCall]._status = WebCall.STATUS.STATUS_CONFERENCEING;
				WebCall.SessionS[otherCall].reinvite({extraHeaders:["P-Conf-MetaData: type=1;join=false"]});
            },
			
			//单步会议
            SingleStepConference:function (activeCall, destination,userdata){
                WebCall.debug("SingleStepConference,activeCall:" + activeCall +",destination:" + destination);
                if (!activeCall || !WebCall.SessionS[activeCall]){
                    WebCall.error("SingleStepConference, the call is not exist.");
                    return 1;
                }
                
				WebCall.SessionS[activeCall]._status = WebCall.STATUS.STATUS_SINGLESTEPCONFERENCEING;
				WebCall.SessionS[activeCall].reinvite({extraHeaders:["P-Conf-MetaData: type=0;user=" + destination + ";join=true",
				"P-User-to-User:" + (userdata ? typeof(userdata) === "string" ? userdata: JSON.stringify(userdata): "")
				]});

            },
            
            // 静音或恢复呼叫
            MuteCall: function () {
                if (WebCall.ASession) {
                    var i_ret;
                    var bMute = !WebCall.ASession.bMute;
                    WebCall.info(bMute ? 'Mute the call...' : 'Unmute the call...');
                    i_ret = WebCall.ASession.mute('audio'/*could be 'video'*/, bMute);
                    if (i_ret != 0) {
                        WebCall.error('Mute / Unmute failed');
                        return;
                    }
                    WebCall.ASession.bMute = bMute;
                }
            },

			setupRemoteMedia:function(session){
				if(!session){
					WebCall.warn('setupRemoteMedia:session is null');
					return;
				}
				
				var audio = session._audio;
				if (!audio){
					audio = document.createElement("audio");
					//audio.setAttribute("id", "audio_remote");
					audio.setAttribute("autoplay", "true");
					document.body.appendChild(audio);
					session._audio = audio;

					session.sessionDescriptionHandler.on('addTrack', function () {
						WebCall.setupRemoteMedia(this);
					}.bind(session));

					session.sessionDescriptionHandler.on('addStream', function () {
						WebCall.setupRemoteMedia(this);
					}.bind(session));
				}
				
				WebCall.debug('setupRemoteMedia:' + session.id.substr(0,session.id.indexOf(session.from_tag)));
				
				var pc = session.sessionDescriptionHandler.peerConnection;
				var remoteStream;

				if (pc.getReceivers) {
					remoteStream = new window.MediaStream();
					pc.getReceivers().forEach(function (receiver) {
						var track = receiver.track;
						if (track) {
						  remoteStream.addTrack(track);
						}
					});
				} else {
					remoteStream = pc.getRemoteStreams()[0];
				}
					
				if (audio) {
					WebCall.debug(session.id.substr(0,session.id.indexOf(session.from_tag)) + ' set audio.srcObject:' + remoteStream);
					audio.srcObject = remoteStream;
					/*audio.play().catch(function (e) {
						WebCall.error('play was rejected:' + e.message);
					}.bind(session));
					*/
				}
					
			},
			
			cleanupMedia:function(session){
				
				if (!session){
					WebCall.warn('cleanupMedia:session is null');
					return;
				}
				
				WebCall.debug('cleanupMedia:' + session.id.substr(0,session.id.indexOf(session.from_tag)));
				if(session._audio) {
					session._audio.srcObject = null;
					session._audio.pause();
				}
			},
			
				
			//获取设备
			getMicroPhoneInfo: function () {

			    var cmd = {};
			    cmd.type = "cmd";
			    cmd.cmd = "getMicroPhoneInfo";
			    //cmd.param = {};
			    WebCall.doSend(cmd);
			},
			
			//设置媒体设备
			selectMicroPhone: function (microId) {
			    var cmd = {};
			    cmd.type = "cmd";
			    cmd.cmd = "selectMicroPhone";
			    cmd.param = {};
			    cmd.param.index = microId;
			    WebCall.doSend(cmd);
			},

            /**
             * 返回当前日期+时间
             * @returns {string}
             */
            dateNow: function () {
                var date = new Date();
                var y = date.getFullYear();
                var m = date.getMonth() + 1;
                var d = date.getDate();
                var h = date.getHours();
                var mm = date.getMinutes();
                var s = date.getSeconds();
                var sss = date.getMilliseconds();
                if (m < 10) {
                    m = "0" + m
                }
                if (d < 10) {
                    d = "0" + d
                }
                if (h < 10) {
                    h = "0" + h
                }
                if (mm < 10) {
                    mm = "0" + mm
                }
                if (s < 10) {
                    s = "0" + s
                }
                return y + "-" + m + "-" + d + " " + h + ":" + mm + ":" + s + "." + sss;
            },
            /**
             * 日志
             * @returns {string}
             */
            trace: function (c) {
                if (WebCall.loglevel == "trace") {
                    c = "[" + WebCall.dateNow() + "] " + c;
                    window.console.trace(c);
                }
            },

            debug: function (c) {
                if(WebCall.loglevel == "trace" || WebCall.loglevel == "debug"){
                    c = "[" + WebCall.dateNow() + "] " + c;
                    window.console.debug(c);
                }
            },
            
            
            info: function (c) {
                if(WebCall.loglevel == "trace" || WebCall.loglevel == "debug" || WebCall.loglevel == "info"){
                    c = "[" + WebCall.dateNow() + "] " + c;
                    window.console.info(c);
                }
            },
            
            warn: function (c) {
                if(WebCall.loglevel == "trace" || WebCall.loglevel == "debug" || WebCall.loglevel == "info"  || WebCall.loglevel == "warn"){
                    c = "[" + WebCall.dateNow() + "] " + c;
                    window.console.warn(c);
                }
            },
            
            error: function (c) {
                if(WebCall.loglevel == "trace" || WebCall.loglevel == "debug" || WebCall.loglevel == "info"  || WebCall.loglevel == "warn" || WebCall.loglevel == "error" ){
                    c = "[" + WebCall.dateNow() + "] " + c;
                    window.console.error(c);
                }
            },

            /**
             * 生成uuid
             * @returns {*}
             */
            getUUID: function () {
                return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function (c) {
                    var r = Math.random() * 16 | 0, v = c == 'x' ? r : (r & 0x3 | 0x8);
                    return v.toString(16);
                });
            },
        }
})();