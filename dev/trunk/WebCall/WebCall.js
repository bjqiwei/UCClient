(function () {

    window.WebCall = window.WebCall || {
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
        setLogLevel: function (level) {
            WebCall.debug("setLogLevel:" + level);
            if (level == "error" || level == "warn" || level == "info" || level == "debug") {
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
                WebCall.error("set log level error, not recognize:" + level);
            }
        },

        /**
         * 初始化
         * @param websocketurl
         */

        Connect: function (websocketurl) {
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

        wsonOpen: function (evt) {
            WebCall.debug("WebSocket CONNECTED");
            if (typeof (WebCall.onWSOpen) == "function") {
                WebCall.onWSOpen();
            }
        },

        wsonClose: function (evt) {
            WebCall.info("WebSocket DISCONNECTED");
            if (typeof (WebCall.onWSClose) == "function") {
                WebCall.onWSClose(evt);
            }
        },

        wsonMessage: function (evt) {

            var event = JSON.parse(evt.data);
            if (event.type == "cmdresult") {
                switch (event.cmdresult) {
                    case "initialize":
                        {
                            WebCall.debug("initialize:" + event.param.return);
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
                    case "disConnectToCCP":
                        {
                            WebCall.info("WebCall disConnectToCCP=" + event.param.return);
                            if (typeof (WebCall.onDisConnectToCCP) == "function") {
                                WebCall.onDisConnectToCCP(event.param.return);
                            }
                        }
                        return;
                    case "makeCall":
                        {
                            WebCall.info("WebCall makeCall=" + event.param.return);
                            if (typeof (WebCall.onMakeCall) == "function") {
                                WebCall.onMakeCall(event.param.return);
                            }
                        }
                        return;
                    case "setUserData":
                        {
                            WebCall.info("WebCall setUserData=" + event.param.return);
                            if (typeof (WebCall.onSetUserData) == "function") {
                                WebCall.onSetUserData(event.param.return);
                            }
                        }
                        return;
                    case "releaseCall":
                        {
                            WebCall.info("WebCall releaseCall=" + event.param.return);
                            if (typeof (WebCall.onReleaseCall) == "function") {
                                WebCall.onReleaseCall(event.param.return);
                            }
                        }
                        return;
                    case "acceptCall":
                        {
                            WebCall.info("WebCall acceptCall=" + event.param.return);
                            if (typeof (WebCall.onAcceptCall) == "function") {
                                WebCall.onAcceptCall(event.param.return);
                            }
                        }
                        return;
                    case "pauseCall":
                        {
                            WebCall.info("WebCall pauseCall=" + event.param.return);
                            if (typeof (WebCall.onPauseCall) == "function") {
                                WebCall.onPauseCall(event.param.return);
                            }
                        }
                        return;
                    case "resumeCall":
                        {
                            WebCall.info("WebCall resumeCall=" + event.param.return);
                            if (typeof (WebCall.onResumeCall) == "function") {
                                WebCall.onResumeCall(event.param.return);
                            }
                        }
                        return;
                    case "transferCall":
                        {
                            WebCall.info("WebCall transferCall=" + event.param.return);
                            if (typeof (WebCall.onTransferCall) == "function") {
                                WebCall.onTransferCall(event.param.return);
                            }
                        }
                        return;
                    case "sendDTMF":
                        {
                            WebCall.info("WebCall sendDTMF=" + event.param.return);
                            if (typeof (WebCall.onSendDTMF) == "function") {
                                WebCall.onSendDTMF(event.param.return);
                            }
                        }
                        return;
                    case "consultTransferCall":
                        {
                            WebCall.info("WebCall consultTransferCall=" + event.param.return);
                            if (typeof (WebCall.onConsultTransferCall) == "function") {
                                WebCall.onConsultTransferCall(event.param.return);
                            }
                        }
                        return;
                    case "transferMeeting":
                        {
                            WebCall.info("WebCall transferMeeting=" + event.param.return);
                            if (typeof (WebCall.onTransferMeeting) == "function") {
                                WebCall.onTransferMeeting(event.param.return);
                            }
                        }
                        return;
                    default:
                        WebCall.warn("WebCall:cmdresult未知命令:" + evt.data);
                        return;
                }
            }
            else if (event.type == "event") {
                WebCall.debug("WebCall:event 事件:" + evt.data);
                switch (event.event) {
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
                        return;
                    case "onSipLogOut":
                        {
                            if (typeof (WebCall.onSipLogOut) == "function") {
                                WebCall.onSipLogOut(event.param);
                            }
                        }
                        return;
                    case "onCallReleased":
                        {
                            if (WebCall.SessionS[event.param.callid]._status == WebCall.STATUS.STATUS_RECONNECTING)
                                event.param.cause = WebCall.Cause.Reconnection;

                            delete WebCall.SessionS[event.param.callid];
                            if (WebCall.callid == event.param.callid)
                                WebCall.callid = null;

                            if (typeof (WebCall.onCallReleased) == "function") {
                                WebCall.onCallReleased(event.param);
                            }
                        }
                        return;
                    case "onCallAnswered":
                        {
                            WebCall.SessionS[event.param.callid] = WebCall.SessionS[event.param.callid] || {};
                            if (WebCall.callid && WebCall.callid != event.param.callid) {
                                WebCall.SessionS[event.param.callid]._cause = WebCall.Cause.Consultation;
                            }
                            event.param.cause = WebCall.SessionS[event.param.callid]._cause;
                            WebCall.callid = event.param.callid;
                            if (typeof (WebCall.onCallAnswered) == "function") {
                                WebCall.onCallAnswered(event.param);
                            }
                        }
                        return;
                    case "onCallAlerting":
                        {
                            WebCall.SessionS[event.param.callid] = WebCall.SessionS[event.param.callid] || {};
                            if (WebCall.callid && WebCall.callid != event.param.callid) {
                                event.param.cause = WebCall.Cause.Consultation;
                                WebCall.SessionS[event.param.callid]._cause = WebCall.Cause.Consultation;
                            }
                            WebCall.callid = event.param.callid;
                            if (typeof (WebCall.onCallAlerting) == "function") {
                                WebCall.onCallAlerting(event.param);
                            }
                        }
                        return;
                    case "onIncomingCallReceived":
                        {
                            WebCall.callid = event.param.callid;
                            WebCall.SessionS[event.param.callid] = WebCall.SessionS[event.param.callid] || {};
                            if (typeof (WebCall.onIncomingCallReceived) == "function") {
                                WebCall.onIncomingCallReceived(event.param);
                            }
                        }
                        return;
                    case "onCallPaused":
                        {
                            if (WebCall.SessionS[event.param.callid]._status == WebCall.STATUS.STATUS_CONSULTATIONING) {
                                WebCall.setUserData(2, WebCall.SessionS[event.param.callid]._userdata);
                                WebCall.makeCall(WebCall.SessionS[event.param.callid]._consultNumber);
                                event.param.cause = WebCall.Cause.Consultation;
                            }

                            if (WebCall.SessionS[event.param.callid]._status == WebCall.STATUS.STATUS_ALTERNATEING)
                                event.param.cause = WebCall.Cause.Alternate;

                            if (typeof (WebCall.onCallPaused) == "function") {
                                WebCall.onCallPaused(event.param);
                            }
                        }
                        return;
                    case "onCallResumed":
                        {
                            WebCall.callid = event.param.callid;
                            if (WebCall.SessionS[event.param.callid]._status === WebCall.STATUS.STATUS_RECONNECTING)
                                WebCall.SessionS[event.param.callid]._status = WebCall.STATUS.STATUS_CONNECTED;

                            if (WebCall.SessionS[event.param.callid]._status == WebCall.STATUS.STATUS_ALTERNATEING)
                                event.param.cause = WebCall.Cause.Alternate;

                            if (typeof (WebCall.onCallResumed) == "function") {
                                WebCall.onCallResumed(event.param);
                            }
                        }
                        return;
                    case "onCallTransfered":
                        {
                            if (typeof (WebCall.onCallTransfered) == "function") {
                                WebCall.onCallTransfered(event.param);
                            }
                        }
                        return;
                    case "onDtmfReceived":
                        {
                            if (typeof (WebCall.onDtmfReceived) == "function") {
                                WebCall.onDtmfReceived(event.param);
                            }
                        }
                        return;
                    case "onMeetingTransfered":
                        {
                            if (typeof (WebCall.onMeetingTransfered) == "function") {
                                WebCall.onMeetingTransfered(event.param);
                            }
                        }
                        return;
                    default:
                        WebCall.warn("WebCall:event未知事件:" + evt.data);
                        return;
                }
            }
            WebCall.warn("WebCall 未知消息：" + evt.data);
        },

        wsonError: function (evt) {
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
        connectToCCPServer: function (server, port, sipid, pwd) {
            // catch exception for IE (DOM not ready)
            WebCall.debug("Login,server:" + server + ":" + port + ",SipID:" + sipid + ",Pwd:" + pwd);

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
        disConnectToCCP: function () {
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "disConnectToCCP";
            WebCall.doSend(cmd);
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

        //设置自定义数据
        setUserData: function (type, data) {
            WebCall.debug("setUserData: type:" + type + ",data:" + data);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "setUserData";
            cmd.param = {};
            cmd.param.type = type;
            cmd.param.data = data;
            WebCall.doSend(cmd);
        },
        //呼叫
        makeCall: function (called) {
            WebCall.debug("makeCall:" + called);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "makeCall";
            cmd.param = {};
            cmd.param.called = called;
            WebCall.doSend(cmd);
        },
        //摘机
        acceptCall: function (callid) {
            WebCall.debug("acceptCall:" + callid);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "acceptCall";
            cmd.param = {};
            cmd.param.callid = callid;
            WebCall.doSend(cmd);
        },

        // 挂断 (SIP BYE or CANCEL)
        releaseCall: function (callid, reason) {
            WebCall.debug("releaseCall, callid:" + callid + ",reason:" + reason);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "releaseCall";
            cmd.param = {};
            cmd.param.callid = callid;
            cmd.param.reason = reason;
            WebCall.doSend(cmd);
        },
        //发送DTMF
        sendDTMF: function (callid, c) {
            WebCall.debug("sendDTMF, callid:" + callid + ",dtmf:" + c);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "sendDTMF";
            cmd.param = {};
            cmd.param.callid = callid;
            cmd.param.dtmf = c;
            WebCall.doSend(cmd);
        },
        // 盲转
        transferCall: function (callid, s_destination) {
            WebCall.debug("transferCall,callid:" + callid + ",destination:" + s_destination);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "transferCall";
            cmd.param = {};
            cmd.param.callid = callid;
            cmd.param.dest = s_destination;
            cmd.param.type = 0;
            WebCall.doSend(cmd);
        },

        /**
         * 保持
         * @param callid
         * @returns {number}
         * @constructor
         */
        pauseCall: function (callid) {
            WebCall.debug("pauseCall,callid:" + callid);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "pauseCall";
            cmd.param = {};
            cmd.param.callid = callid;
            WebCall.doSend(cmd);
        },
        /**
         * 恢复
         * @param callid
         * @returns {number}
         * @constructor
         */
        resumeCall: function (callid) {
            WebCall.debug("resumeCall,callid:" + callid);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "resumeCall";
            cmd.param = {};
            cmd.param.callid = callid;
            WebCall.doSend(cmd);
        },

        //咨询
        ConsultationCall: function (callid, called, userdata) {
            WebCall.debug("ConsultationCall,callid:" + callid + ",called:" + called);
            if (callid && WebCall.SessionS[callid]) {
                WebCall.SessionS[callid]._status = WebCall.STATUS.STATUS_CONSULTATIONING;
                WebCall.SessionS[callid]._consultNumber = called;
                WebCall.SessionS[callid]._userdata = userdata;
                WebCall.pauseCall(callid);
                WebCall.debug('pauseCall the call...' + callid);
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
            WebCall.debug("ReconnectCall,activeCall:" + activeCall + ",heldCall:" + heldCall);

            if (activeCall && WebCall.SessionS[activeCall]) {
                WebCall.SessionS[activeCall]._status = WebCall.STATUS.STATUS_RECONNECTING;
                WebCall.releaseCall(activeCall);
            }

            if (heldCall && WebCall.SessionS[heldCall]) {
                WebCall.SessionS[heldCall]._status = WebCall.STATUS.STATUS_RECONNECTING;
                WebCall.debug('resumeCall the call...' + heldCall);
                WebCall.resumeCall(heldCall);
            }
            else {
                WebCall.error("ReconnectCall, the call is not exist.");
            }

        },

        // 咨询后转接
        consultTransferCall:function(callid, consultCallid, destination){
            WebCall.debug("consultTransferCall,callid:" + callid + ", consultCallid:" + consultCallid + ",destination:" + destination);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "consultTransferCall";
            cmd.param = {};
            cmd.param.callid = callid;
            cmd.param.consultCallid = consultCallid;
            cmd.param.dest = destination;
            cmd.param.type = 0;
            WebCall.doSend(cmd);
        },
        
        //切换通话
        AlternateCall: function (activeCall, otherCall) {
            WebCall.debug("AlternateCall,activeCall:" + activeCall + ",otherCall:" + otherCall);
            if (!activeCall || !WebCall.SessionS[activeCall]) {
                WebCall.error("AlternateCall, the activeCall is not exist.");
                return 1;
            }

            if (!otherCall || !WebCall.SessionS[otherCall]) {
                WebCall.error("AlternateCall, the otherCall is not exist.");
                return 1;
            }
            WebCall.SessionS[activeCall]._status = WebCall.STATUS.STATUS_ALTERNATEING;
            WebCall.SessionS[otherCall]._status = WebCall.STATUS.STATUS_ALTERNATEING;
            WebCall.pauseCall(activeCall);
            WebCall.resumeCall(otherCall);
        },

        //会议
        transferMeeting: function (type, activeCall, otherCall, consultedUser) {
            WebCall.debug("transferMeeting, type:" + type + ",activeCall:" + activeCall + ",otherCall:" + otherCall + ",consultedUser:" + consultedUser);
            if (activeCall && WebCall.SessionS[activeCall])
                WebCall.SessionS[activeCall]._status = WebCall.STATUS.STATUS_CONFERENCEING;

            if (otherCall && WebCall.SessionS[otherCall])
                WebCall.SessionS[otherCall]._status = WebCall.STATUS.STATUS_CONFERENCEING;
            
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "transferMeeting";
            cmd.param = {};
            cmd.param.callid = activeCall;
            cmd.param.consultCallid = otherCall;
            cmd.param.consultedUser = consultedUser;
            cmd.param.type = type;
            WebCall.doSend(cmd);
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
            if (WebCall.loglevel == "trace" || WebCall.loglevel == "debug") {
                c = "[" + WebCall.dateNow() + "] " + c;
                window.console.debug(c);
            }
        },


        info: function (c) {
            if (WebCall.loglevel == "trace" || WebCall.loglevel == "debug" || WebCall.loglevel == "info") {
                c = "[" + WebCall.dateNow() + "] " + c;
                window.console.info(c);
            }
        },

        warn: function (c) {
            if (WebCall.loglevel == "trace" || WebCall.loglevel == "debug" || WebCall.loglevel == "info" || WebCall.loglevel == "warn") {
                c = "[" + WebCall.dateNow() + "] " + c;
                window.console.warn(c);
            }
        },

        error: function (c) {
            if (WebCall.loglevel == "trace" || WebCall.loglevel == "debug" || WebCall.loglevel == "info" || WebCall.loglevel == "warn" || WebCall.loglevel == "error") {
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