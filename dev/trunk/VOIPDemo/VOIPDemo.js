(function () {

    window.VOIPDemo = window.VOIPDemo || {
        callid: null,
        SessionS: [],
        loglevel: "debug",
        websocketurl: null,
        websocket: null,
        STATUS: {
            // call states
            STATUS_NULL: 0,
            STATUS_CONSULTATIONING: 1,
            STATUS_RECONNECTING: 2,
            STATUS_CONNECTED: 3,
            STATUS_ALTERNATEING: 4,
            STATUS_CONFERENCEING: 5,
            STATUS_SINGLESTEPCONFERENCEING: 6
        },

        Cause: {
            Alternate: 0,
            CallCancelled: 1,
            CallNotAnswered: 2,
            Consultation: 3,
            Reconnection:4,
            MakeCall: 5,
            NewCall: 6,
            NormalClearing: 7,
            SingleStepConference: 8,
            Conference: 9,
            SingleStepTransfer: 10,
            Transfer: 11
        },
        /**
        *设置日志级别
        * @param level:error,warn,info,debug
        **/
        setTraceFlag: function (enable, logFileName, level) {
            VOIPDemo.debug("setTraceFlag:" + level);

            VOIPDemo.loglevel = "debug";
            //SIP.setDebugLevel(VOIPDemo.loglevel);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "setTraceFlag";
            cmd.param = {};
            cmd.param.enable = enable;
            cmd.param.logFileName = logFileName;
            cmd.param.logLevel = level;
            VOIPDemo.doSend(cmd);

        },

        /**
         * 初始化
         * @param websocketurl
         */

        Connect: function (websocketurl) {
            if (typeof (websocketurl) == "undefined") {
                VOIPDemo.error("websocketurl not init");
                return false;
            }

            VOIPDemo.websocketurl = websocketurl;
            VOIPDemo.debug("Connect websocketurl:" + VOIPDemo.websocketurl);
            VOIPDemo.websocket = new window.WebSocket(VOIPDemo.websocketurl);

            VOIPDemo.websocket.onopen = VOIPDemo.wsonOpen;
            VOIPDemo.websocket.onclose = VOIPDemo.wsonClose;
            VOIPDemo.websocket.onmessage = VOIPDemo.wsonMessage;
            VOIPDemo.websocket.onerror = VOIPDemo.wsonError;

        },

        setSipTransportType:function(type)
        {
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "setSipTransportType";
            cmd.param = {};
            cmd.param.transType = type;
            VOIPDemo.doSend(cmd);
        },

        setSrtpEnabled:function(TransportType,cryptoType)
        {
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "setSrtpEnabled";
            cmd.param = {};
            cmd.param.TransportType = TransportType;
            cmd.param.cryptoType = cryptoType;
            VOIPDemo.doSend(cmd);
        },

        /**
         * 初始化
         */
        initialize: function () {
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "initialize";
            //cmd.param = {};
            VOIPDemo.doSend(cmd);
            return true;
        },

        wsonOpen: function (evt) {
            VOIPDemo.debug("WebSocket CONNECTED");
            if (typeof (VOIPDemo.onWSOpen) == "function") {
                VOIPDemo.onWSOpen();
            }
        },

        wsonClose: function (evt) {
            VOIPDemo.info("WebSocket DISCONNECTED");
            if (typeof (VOIPDemo.onWSClose) == "function") {
                VOIPDemo.onWSClose(evt);
            }
        },

        wsonMessage: function (evt) {

            var event = JSON.parse(evt.data);
            if (event.type == "cmdresult") {
                switch (event.cmdresult) {
                    case "initialize":
                        {
                            VOIPDemo.debug("initialize:" + event.param.return);
                            if (typeof (VOIPDemo.onInitialize) == "function") {
                                VOIPDemo.onInitialize(event.param.return);
                            }
                            VOIPDemo.getVersion();
                        }
                        return;
                    case "getVersion":
                        {
                            VOIPDemo.info("VOIPDemo version=" + event.param.return);
                            if (typeof (VOIPDemo.onGetVersion) == "function") {
                                VOIPDemo.onGetVersion(event.param.return);
                            }
                        }
                        return;
                    case "getMicroPhoneInfo":
                        {
                            VOIPDemo.info("VOIPDemo micro info=" + JSON.stringify(event.param.return));
                            if (typeof (VOIPDemo.onGetMicroPhoneInfo) == "function") {
                                VOIPDemo.onGetMicroPhoneInfo(event.param.return);
                            }
                        }
                        return;
                    case "selectMicroPhone":
                        {
                            VOIPDemo.info("VOIPDemo selectMicroPhone=" + event.param.return);
                            if (typeof (VOIPDemo.onSelectMicroPhone) == "function") {
                                VOIPDemo.onSelectMicroPhone(event.param.return);
                            }
                        }
                        return;
                    case "setTraceFlag":
                        {
                            VOIPDemo.info("VOIPDemo setTraceFlag=" + event.param.return);
                            if (typeof (VOIPDemo.onSetTraceFlag) == "function") {
                                VOIPDemo.onSetTraceFlag(event.param.return);
                            }
                        }
                        return;
                    case "connectToCCPServer":
                        {
                            VOIPDemo.info("VOIPDemo connectToCCPServer=" + event.param.return);
                            if (typeof (VOIPDemo.onConnectToCCPServer) == "function") {
                                VOIPDemo.onConnectToCCPServer(event.param.return);
                            }
                        }
                        return;
                    case "disConnectToCCP":
                        {
                            VOIPDemo.info("VOIPDemo disConnectToCCP=" + event.param.return);
                            if (typeof (VOIPDemo.onDisConnectToCCP) == "function") {
                                VOIPDemo.onDisConnectToCCP(event.param.return);
                            }
                        }
                        return;
                    case "makeCall":
                        {
                            VOIPDemo.info("VOIPDemo makeCall=" + event.param.return);
                            if (typeof (VOIPDemo.onMakeCall) == "function") {
                                VOIPDemo.onMakeCall(event.param.return);
                            }
                        }
                        return;
                    case "setUserData":
                        {
                            VOIPDemo.info("VOIPDemo setUserData=" + event.param.return);
                            if (typeof (VOIPDemo.onSetUserData) == "function") {
                                VOIPDemo.onSetUserData(event.param.return);
                            }
                        }
                        return;
                    case "releaseCall":
                        {
                            VOIPDemo.info("VOIPDemo releaseCall=" + event.param.return);
                            if (typeof (VOIPDemo.onReleaseCall) == "function") {
                                VOIPDemo.onReleaseCall(event.param.return);
                            }
                        }
                        return;
                    case "acceptCall":
                        {
                            VOIPDemo.info("VOIPDemo acceptCall=" + event.param.return);
                            if (typeof (VOIPDemo.onAcceptCall) == "function") {
                                VOIPDemo.onAcceptCall(event.param.return);
                            }
                        }
                        return;
                    case "pauseCall":
                        {
                            VOIPDemo.info("VOIPDemo pauseCall=" + event.param.return);
                            if (typeof (VOIPDemo.onPauseCall) == "function") {
                                VOIPDemo.onPauseCall(event.param.return);
                            }
                        }
                        return;
                    case "resumeCall":
                        {
                            VOIPDemo.info("VOIPDemo resumeCall=" + event.param.return);
                            if (typeof (VOIPDemo.onResumeCall) == "function") {
                                VOIPDemo.onResumeCall(event.param.return);
                            }
                        }
                        return;
                    case "transferCall":
                        {
                            VOIPDemo.info("VOIPDemo transferCall=" + event.param.return);
                            if (typeof (VOIPDemo.onTransferCall) == "function") {
                                VOIPDemo.onTransferCall(event.param.return);
                            }
                        }
                        return;
                    case "sendDTMF":
                        {
                            VOIPDemo.info("VOIPDemo sendDTMF=" + event.param.return);
                            if (typeof (VOIPDemo.onSendDTMF) == "function") {
                                VOIPDemo.onSendDTMF(event.param.return);
                            }
                        }
                        return;
                    case "consultTransferCall":
                        {
                            VOIPDemo.info("VOIPDemo consultTransferCall=" + event.param.return);
                            if (typeof (VOIPDemo.onConsultTransferCall) == "function") {
                                VOIPDemo.onConsultTransferCall(event.param.return);
                            }
                        }
                        return;
                    case "transferMeeting":
                        {
                            VOIPDemo.info("VOIPDemo transferMeeting=" + event.param.return);
                            if (typeof (VOIPDemo.onTransferMeeting) == "function") {
                                VOIPDemo.onTransferMeeting(event.param.return);
                            }
                        }
                        return;
                    case "setSipTransportType":
                        {
                            VOIPDemo.info("VOIPDemo setSipTransportType=" + event.param.return);
                            if (typeof (VOIPDemo.onSetSipTransportType) == "function") {
                                VOIPDemo.onSetSipTransportType(event.param.return);
                            }
                        }
                        return;
                    case "setSrtpEnabled":
                        {
                            VOIPDemo.info("VOIPDemo setSrtpEnabled");
                            if (typeof (VOIPDemo.onSetSrtpEnabled) == "function") {
                                VOIPDemo.onSetSrtpEnabled();
                            }
                        }
                        return;
                    default:
                        VOIPDemo.warn("VOIPDemo:cmdresult未知命令:" + evt.data);
                        return;
                }
            }
            else if (event.type == "event") {
                VOIPDemo.debug("VOIPDemo:event 事件:" + evt.data);
                switch (event.event) {
                    case "onLogInfo":
                        {
                            VOIPDemo.trace(evt.data);
                        }
                        return;
                    case "onSipConnect":
                        {
                            if (typeof (VOIPDemo.onSipConnect) == "function") {
                                VOIPDemo.onSipConnect(event.param);
                            }
                        }
                        return;
                    case "onSipLogOut":
                        {
                            if (typeof (VOIPDemo.onSipLogOut) == "function") {
                                VOIPDemo.onSipLogOut(event.param);
                            }
                        }
                        return;
                    case "onCallReleased":
                        {
                            if (VOIPDemo.SessionS[event.param.callid] && VOIPDemo.SessionS[event.param.callid]._status == VOIPDemo.STATUS.STATUS_RECONNECTING)
                                event.param.cause = VOIPDemo.Cause.Reconnection;

                            delete VOIPDemo.SessionS[event.param.callid];
                            if (VOIPDemo.callid == event.param.callid)
                                VOIPDemo.callid = null;

                            if (typeof (VOIPDemo.onCallReleased) == "function") {
                                VOIPDemo.onCallReleased(event.param);
                            }
                        }
                        return;
                    case "onCallAnswered":
                        {
                            VOIPDemo.SessionS[event.param.callid] = VOIPDemo.SessionS[event.param.callid] || {};
                            if (VOIPDemo.callid && VOIPDemo.callid != event.param.callid) {
                                VOIPDemo.SessionS[event.param.callid]._cause = VOIPDemo.Cause.Consultation;
                            }
                            event.param.cause = VOIPDemo.SessionS[event.param.callid]._cause;
                            VOIPDemo.callid = event.param.callid;
                            if (typeof (VOIPDemo.onCallAnswered) == "function") {
                                VOIPDemo.onCallAnswered(event.param);
                            }
                        }
                        return;
                    case "onCallAlerting":
                        {
                            VOIPDemo.SessionS[event.param.callid] = VOIPDemo.SessionS[event.param.callid] || {};
                            if (VOIPDemo.callid && VOIPDemo.callid != event.param.callid) {
                                event.param.cause = VOIPDemo.Cause.Consultation;
                                VOIPDemo.SessionS[event.param.callid]._cause = VOIPDemo.Cause.Consultation;
                            }
                            VOIPDemo.callid = event.param.callid;
                            if (typeof (VOIPDemo.onCallAlerting) == "function") {
                                VOIPDemo.onCallAlerting(event.param);
                            }
                        }
                        return;
                    case "onIncomingCallReceived":
                        {
                            VOIPDemo.callid = event.param.callid;
                            VOIPDemo.SessionS[event.param.callid] = VOIPDemo.SessionS[event.param.callid] || {};
                            if (typeof (VOIPDemo.onIncomingCallReceived) == "function") {
                                VOIPDemo.onIncomingCallReceived(event.param);
                            }
                        }
                        return;
                    case "onCallPaused":
                        {
                            if (VOIPDemo.SessionS[event.param.callid]._status == VOIPDemo.STATUS.STATUS_CONSULTATIONING) {
                                VOIPDemo.setUserData(2, VOIPDemo.SessionS[event.param.callid]._userdata);
                                VOIPDemo.makeCall(VOIPDemo.SessionS[event.param.callid]._consultNumber);
                                event.param.cause = VOIPDemo.Cause.Consultation;
                            }

                            if (VOIPDemo.SessionS[event.param.callid]._status == VOIPDemo.STATUS.STATUS_ALTERNATEING)
                                event.param.cause = VOIPDemo.Cause.Alternate;

                            if (typeof (VOIPDemo.onCallPaused) == "function") {
                                VOIPDemo.onCallPaused(event.param);
                            }
                        }
                        return;
                    case "onCallResumed":
                        {
                            VOIPDemo.callid = event.param.callid;
                            if (VOIPDemo.SessionS[event.param.callid]._status === VOIPDemo.STATUS.STATUS_RECONNECTING)
                                VOIPDemo.SessionS[event.param.callid]._status = VOIPDemo.STATUS.STATUS_CONNECTED;

                            if (VOIPDemo.SessionS[event.param.callid]._status == VOIPDemo.STATUS.STATUS_ALTERNATEING)
                                event.param.cause = VOIPDemo.Cause.Alternate;

                            if (typeof (VOIPDemo.onCallResumed) == "function") {
                                VOIPDemo.onCallResumed(event.param);
                            }
                        }
                        return;
                    case "onCallTransfered":
                        {
                            if (typeof (VOIPDemo.onCallTransfered) == "function") {
                                VOIPDemo.onCallTransfered(event.param);
                            }
                        }
                        return;
                    case "onDtmfReceived":
                        {
                            if (typeof (VOIPDemo.onDtmfReceived) == "function") {
                                VOIPDemo.onDtmfReceived(event.param);
                            }
                        }
                        return;
                    case "onMeetingTransfered":
                        {
                            if (typeof (VOIPDemo.onMeetingTransfered) == "function") {
                                VOIPDemo.onMeetingTransfered(event.param);
                            }
                        }
                        return;
                    default:
                        VOIPDemo.warn("VOIPDemo:event未知事件:" + evt.data);
                        return;
                }
            }
            VOIPDemo.warn("VOIPDemo 未知消息：" + evt.data);
        },

        wsonError: function (evt) {
            VOIPDemo.error(evt.data);
            if (typeof (VOIPDemo.onWSError) == "function") {
                VOIPDemo.onWSError(evt?evt:"连接失败");
            }
        },

        doSend: function (cmd) {
            var message = JSON.stringify(cmd)
            VOIPDemo.debug("SENT: " + message);
            VOIPDemo.websocket.send(message);
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
        connectToCCPServer: function (server, port, sipid, pwd) {
            // catch exception for IE (DOM not ready)
            VOIPDemo.debug("Login,server:" + server + ":" + port + ",SipID:" + sipid + ",Pwd:" + pwd);

            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "connectToCCPServer";
            cmd.param = {};
            cmd.param.serverAddr = server;
            cmd.param.serverPort = parseInt(port);
            cmd.param.voipId = sipid;
            cmd.param.voipPwd = pwd;
            VOIPDemo.doSend(cmd);
        },
        // 注销
        disConnectToCCP: function () {
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "disConnectToCCP";
            VOIPDemo.doSend(cmd);
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
            VOIPDemo.doSend(cmd);
        },

        //设置自定义数据
        setUserData: function (type, data) {
            VOIPDemo.debug("setUserData: type:" + type + ",data:" + data);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "setUserData";
            cmd.param = {};
            cmd.param.type = type;
            cmd.param.data = data;
            VOIPDemo.doSend(cmd);
        },
        //呼叫
        makeCall: function (called) {
            VOIPDemo.debug("makeCall:" + called);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "makeCall";
            cmd.param = {};
            cmd.param.called = called;
            VOIPDemo.doSend(cmd);
        },
        //摘机
        acceptCall: function (callid) {
            VOIPDemo.debug("acceptCall:" + callid);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "acceptCall";
            cmd.param = {};
            cmd.param.callid = callid;
            VOIPDemo.doSend(cmd);
        },

        // 挂断 (SIP BYE or CANCEL)
        releaseCall: function (callid, reason) {
            VOIPDemo.debug("releaseCall, callid:" + callid + ",reason:" + reason);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "releaseCall";
            cmd.param = {};
            cmd.param.callid = callid;
            cmd.param.reason = reason;
            VOIPDemo.doSend(cmd);
        },
        //发送DTMF
        sendDTMF: function (callid, c) {
            VOIPDemo.debug("sendDTMF, callid:" + callid + ",dtmf:" + c);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "sendDTMF";
            cmd.param = {};
            cmd.param.callid = callid;
            cmd.param.dtmf = c;
            VOIPDemo.doSend(cmd);
        },
        // 盲转
        transferCall: function (callid, s_destination) {
            VOIPDemo.debug("transferCall,callid:" + callid + ",destination:" + s_destination);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "transferCall";
            cmd.param = {};
            cmd.param.callid = callid;
            cmd.param.dest = s_destination;
            cmd.param.type = 0;
            VOIPDemo.doSend(cmd);
        },

        /**
         * 保持
         * @param callid
         * @returns {number}
         * @constructor
         */
        pauseCall: function (callid) {
            VOIPDemo.debug("pauseCall,callid:" + callid);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "pauseCall";
            cmd.param = {};
            cmd.param.callid = callid;
            VOIPDemo.doSend(cmd);
        },
        /**
         * 恢复
         * @param callid
         * @returns {number}
         * @constructor
         */
        resumeCall: function (callid) {
            VOIPDemo.debug("resumeCall,callid:" + callid);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "resumeCall";
            cmd.param = {};
            cmd.param.callid = callid;
            VOIPDemo.doSend(cmd);
        },

        //咨询
        ConsultationCall: function (callid, called, userdata) {
            VOIPDemo.debug("ConsultationCall,callid:" + callid + ",called:" + called);
            if (callid && VOIPDemo.SessionS[callid]) {
                VOIPDemo.SessionS[callid]._status = VOIPDemo.STATUS.STATUS_CONSULTATIONING;
                VOIPDemo.SessionS[callid]._consultNumber = called;
                VOIPDemo.SessionS[callid]._userdata = userdata;
                VOIPDemo.pauseCall(callid);
                VOIPDemo.debug('pauseCall the call...' + callid);
                //VOIPDemo.MakeCall(called);                    
                return 0;

            } else {
                VOIPDemo.error("ConsultationCall, the call is not exist.");
                return 3;
            }
        },

        /**
         * 取消咨询
         * @param callid
         * @constructor
         */
        ReconnectCall: function (activeCall, heldCall) {
            VOIPDemo.debug("ReconnectCall,activeCall:" + activeCall + ",heldCall:" + heldCall);

            if (activeCall && VOIPDemo.SessionS[activeCall]) {
                VOIPDemo.SessionS[activeCall]._status = VOIPDemo.STATUS.STATUS_RECONNECTING;
                VOIPDemo.releaseCall(activeCall);
            }

            if (heldCall && VOIPDemo.SessionS[heldCall]) {
                VOIPDemo.SessionS[heldCall]._status = VOIPDemo.STATUS.STATUS_RECONNECTING;
                VOIPDemo.debug('resumeCall the call...' + heldCall);
                VOIPDemo.resumeCall(heldCall);
            }
            else {
                VOIPDemo.error("ReconnectCall, the call is not exist.");
            }

        },

        // 咨询后转接
        consultTransferCall:function(callid, consultCallid, destination){
            VOIPDemo.debug("consultTransferCall,callid:" + callid + ", consultCallid:" + consultCallid + ",destination:" + destination);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "consultTransferCall";
            cmd.param = {};
            cmd.param.callid = callid;
            cmd.param.consultCallid = consultCallid;
            cmd.param.dest = destination;
            cmd.param.type = 0;
            VOIPDemo.doSend(cmd);
        },
        
        //切换通话
        AlternateCall: function (activeCall, otherCall) {
            VOIPDemo.debug("AlternateCall,activeCall:" + activeCall + ",otherCall:" + otherCall);
            if (!activeCall || !VOIPDemo.SessionS[activeCall]) {
                VOIPDemo.error("AlternateCall, the activeCall is not exist.");
                return 1;
            }

            if (!otherCall || !VOIPDemo.SessionS[otherCall]) {
                VOIPDemo.error("AlternateCall, the otherCall is not exist.");
                return 1;
            }
            VOIPDemo.SessionS[activeCall]._status = VOIPDemo.STATUS.STATUS_ALTERNATEING;
            VOIPDemo.SessionS[otherCall]._status = VOIPDemo.STATUS.STATUS_ALTERNATEING;
            VOIPDemo.pauseCall(activeCall);
            VOIPDemo.resumeCall(otherCall);
        },

        //会议
        transferMeeting: function (type, activeCall, otherCall, consultedUser) {
            VOIPDemo.debug("transferMeeting, type:" + type + ",activeCall:" + activeCall + ",otherCall:" + otherCall + ",consultedUser:" + consultedUser);
            if (activeCall && VOIPDemo.SessionS[activeCall])
                VOIPDemo.SessionS[activeCall]._status = VOIPDemo.STATUS.STATUS_CONFERENCEING;

            if (otherCall && VOIPDemo.SessionS[otherCall])
                VOIPDemo.SessionS[otherCall]._status = VOIPDemo.STATUS.STATUS_CONFERENCEING;
            
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "transferMeeting";
            cmd.param = {};
            cmd.param.callid = activeCall;
            cmd.param.consultCallid = otherCall;
            cmd.param.consultedUser = consultedUser;
            cmd.param.type = type;
            VOIPDemo.doSend(cmd);
        },


        //获取设备
        getMicroPhoneInfo: function () {

            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "getMicroPhoneInfo";
            //cmd.param = {};
            VOIPDemo.doSend(cmd);
        },

        //设置媒体设备
        selectMicroPhone: function (microId) {
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "selectMicroPhone";
            cmd.param = {};
            cmd.param.index = microId;
            VOIPDemo.doSend(cmd);
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
            if (VOIPDemo.loglevel == "trace") {
                c = "[" + VOIPDemo.dateNow() + "] " + c;
                window.console.trace(c);
            }
        },

        debug: function (c) {
            if (VOIPDemo.loglevel == "trace" || VOIPDemo.loglevel == "debug") {
                c = "[" + VOIPDemo.dateNow() + "] " + c;
                window.console.debug(c);
            }
        },


        info: function (c) {
            if (VOIPDemo.loglevel == "trace" || VOIPDemo.loglevel == "debug" || VOIPDemo.loglevel == "info") {
                c = "[" + VOIPDemo.dateNow() + "] " + c;
                window.console.info(c);
            }
        },

        warn: function (c) {
            if (VOIPDemo.loglevel == "trace" || VOIPDemo.loglevel == "debug" || VOIPDemo.loglevel == "info" || VOIPDemo.loglevel == "warn") {
                c = "[" + VOIPDemo.dateNow() + "] " + c;
                window.console.warn(c);
            }
        },

        error: function (c) {
            if (VOIPDemo.loglevel == "trace" || VOIPDemo.loglevel == "debug" || VOIPDemo.loglevel == "info" || VOIPDemo.loglevel == "warn" || VOIPDemo.loglevel == "error") {
                c = "[" + VOIPDemo.dateNow() + "] " + c;
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