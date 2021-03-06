(function () {

    window.UCClient = window.UCClient || {
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
            UCClient.debug("setTraceFlag:" + level);

            UCClient.loglevel = "debug";
            //SIP.setDebugLevel(UCClient.loglevel);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "setTraceFlag";
            cmd.param = {};
            cmd.param.enable = enable;
            cmd.param.logFileName = logFileName;
            cmd.param.logLevel = level;
            UCClient.doSend(cmd);

        },

        /**
         * 初始化
         * @param websocketurl
         */

        Connect: function (websocketurl) {
            if (typeof (websocketurl) == "undefined") {
                UCClient.error("websocketurl not init");
                return false;
            }

            UCClient.websocketurl = websocketurl;
            UCClient.debug("Connect websocketurl:" + UCClient.websocketurl);
            UCClient.websocket = new window.WebSocket(UCClient.websocketurl);

            UCClient.websocket.onopen = UCClient.wsonOpen;
            UCClient.websocket.onclose = UCClient.wsonClose;
            UCClient.websocket.onmessage = UCClient.wsonMessage;
            UCClient.websocket.onerror = UCClient.wsonError;

        },

        setSipTransportType:function(type)
        {
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "setSipTransportType";
            cmd.param = {};
            cmd.param.transType = type;
            UCClient.doSend(cmd);
        },

        setSrtpEnabled:function(TransportType,cryptoType)
        {
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "setSrtpEnabled";
            cmd.param = {};
            cmd.param.TransportType = TransportType;
            cmd.param.cryptoType = cryptoType;
            UCClient.doSend(cmd);
        },

        /**
         * 初始化
         */
        initialize: function () {
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "initialize";
            //cmd.param = {};
            UCClient.doSend(cmd);
            return true;
        },

        /**
     * 销毁
     */
        unInitialize: function () {
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "unInitialize";
            //cmd.param = {};
            UCClient.doSend(cmd);
        },

        wsonOpen: function (evt) {
            UCClient.debug("WebSocket CONNECTED");
            if (typeof (UCClient.onWSOpen) == "function") {
                UCClient.onWSOpen();
            }
        },

        wsonClose: function (evt) {
            UCClient.info("WebSocket DISCONNECTED");
            if (typeof (UCClient.onWSClose) == "function") {
                UCClient.onWSClose(evt);
            }
        },

        wsonMessage: function (evt) {

            var event = JSON.parse(evt.data);
            if (event.type == "cmdresult") {
                switch (event.cmdresult) {
                    case "initialize":
                        {
                            UCClient.debug("initialize:" + event.param.return);
                            if (typeof (UCClient.oninitialize) == "function") {
                                UCClient.oninitialize(event.param.return);
                            }
                        }
                        return;
                    case "unInitialize":
                        {
                            UCClient.debug("unInitialize:" + event.param.return);
                            if (typeof (UCClient.onunInitialize) == "function") {
                                UCClient.onunInitialize(event.param.return);
                            }
                        }
                        return;
                    case "getVersion":
                        {
                            UCClient.info("version=" + event.param.return);
                            if (typeof (UCClient.ongetVersion) == "function") {
                                UCClient.ongetVersion(event.param.return);
                            }
                        }
                        return;
                    case "getMicroPhoneInfo":
                        {
                            UCClient.info("micro info=" + JSON.stringify(event.param.return));
                            if (typeof (UCClient.ongetMicroPhoneInfo) == "function") {
                                UCClient.ongetMicroPhoneInfo(event.param.return);
                            }
                        }
                        return;
                    case "selectMicroPhone":
                        {
                            UCClient.info("selectMicroPhone=" + event.param.return);
                            if (typeof (UCClient.onselectMicroPhone) == "function") {
                                UCClient.onselectMicroPhone(event.param.return);
                            }
                        }
                        return;
                    case "getSpeakerInfo":
                        {
                            UCClient.info("speaker info=" + JSON.stringify(event.param.return));
                            if (typeof (UCClient.ongetSpeakerInfo) == "function") {
                                UCClient.ongetSpeakerInfo(event.param.return);
                            }
                        }
                        return;
                    case "selectSpeaker":
                        {
                            UCClient.info("selectSpeaker=" + JSON.stringify(event.param.return));
                            if (typeof (UCClient.onselectSpeaker) == "function") {
                                UCClient.onselectSpeaker(event.param.return);
                            }
                        }
                        return;
                    case "setTraceFlag":
                        {
                            UCClient.info("setTraceFlag=" + event.param.return);
                            if (typeof (UCClient.onsetTraceFlag) == "function") {
                                UCClient.onsetTraceFlag(event.param.return);
                            }
                        }
                        return;
                    case "connectToCCPServer":
                        {
                            UCClient.info("connectToCCPServer=" + event.param.return);
                            if (typeof (UCClient.onconnectToCCPServer) == "function") {
                                UCClient.onconnectToCCPServer(event.param.return);
                            }
                        }
                        return;
                    case "disConnectToCCP":
                        {
                            UCClient.info("disConnectToCCP=" + event.param.return);
                            if (typeof (UCClient.ondisConnectToCCP) == "function") {
                                UCClient.ondisConnectToCCP(event.param.return);
                            }
                        }
                        return;
                    case "makeCall":
                        {
                            UCClient.info("makeCall=" + event.param.return);
                            if (typeof (UCClient.onmakeCall) == "function") {
                                UCClient.onmakeCall(event.param.return);
                            }
                        }
                        return;
                    case "setUserData":
                        {
                            UCClient.info("setUserData=" + event.param.return);
                            if (typeof (UCClient.onsetUserData) == "function") {
                                UCClient.onsetUserData(event.param.return);
                            }
                        }
                        return;
                    case "releaseCall":
                        {
                            UCClient.info("releaseCall=" + event.param.return);
                            if (typeof (UCClient.onreleaseCall) == "function") {
                                UCClient.onreleaseCall(event.param.return);
                            }
                        }
                        return;
                    case "acceptCall":
                        {
                            UCClient.info("acceptCall=" + event.param.return);
                            if (typeof (UCClient.onacceptCall) == "function") {
                                UCClient.onacceptCall(event.param.return);
                            }
                        }
                        return;
                    case "pauseCall":
                        {
                            UCClient.info("pauseCall=" + event.param.return);
                            if (typeof (UCClient.onpauseCall) == "function") {
                                UCClient.onpauseCall(event.param.return);
                            }
                        }
                        return;
                    case "resumeCall":
                        {
                            UCClient.info("resumeCall=" + event.param.return);
                            if (typeof (UCClient.onresumeCall) == "function") {
                                UCClient.onresumeCall(event.param.return);
                            }
                        }
                        return;
                    case "transferCall":
                        {
                            UCClient.info("transferCall=" + event.param.return);
                            if (typeof (UCClient.ontransferCall) == "function") {
                                UCClient.ontransferCall(event.param.return);
                            }
                        }
                        return;
                    case "sendDTMF":
                        {
                            UCClient.info("sendDTMF=" + event.param.return);
                            if (typeof (UCClient.onsendDTMF) == "function") {
                                UCClient.onsendDTMF(event.param.return);
                            }
                        }
                        return;
                    case "consultTransferCall":
                        {
                            UCClient.info("consultTransferCall=" + event.param.return);
                            if (typeof (UCClient.onconsultTransferCall) == "function") {
                                UCClient.onconsultTransferCall(event.param.return);
                            }
                        }
                        return;
                    case "transferMeeting":
                        {
                            UCClient.info("transferMeeting=" + event.param.return);
                            if (typeof (UCClient.ontransferMeeting) == "function") {
                                UCClient.ontransferMeeting(event.param.return);
                            }
                        }
                        return;
                    case "setSipTransportType":
                        {
                            UCClient.info("setSipTransportType=" + event.param.return);
                            if (typeof (UCClient.onsetSipTransportType) == "function") {
                                UCClient.onsetSipTransportType(event.param.return);
                            }
                        }
                        return;
                    case "setSrtpEnabled":
                        {
                            UCClient.info("setSrtpEnabled");
                            if (typeof (UCClient.onsetSrtpEnabled) == "function") {
                                UCClient.onsetSrtpEnabled();
                            }
                        }
                        return;
                    default:
                        UCClient.warn("cmdresult未知命令:" + evt.data);
                        return;
                }
            }
            else if (event.type == "event") {
                UCClient.debug("event 事件:" + evt.data);
                switch (event.event) {
                    case "onLogInfo":
                        {
                            UCClient.trace(evt.data);
                        }
                        return;
                    case "onSipConnect":
                        {
                            if (typeof (UCClient.onSipConnect) == "function") {
                                UCClient.onSipConnect(event.param);
                            }
                        }
                        return;
                    case "onSipLogOut":
                        {
                            if (typeof (UCClient.onSipLogOut) == "function") {
                                UCClient.onSipLogOut(event.param);
                            }
                        }
                        return;
                    case "onCallReleased":
                        {
                            if (UCClient.SessionS[event.param.callid] && UCClient.SessionS[event.param.callid]._status == UCClient.STATUS.STATUS_RECONNECTING)
                                event.param.cause = UCClient.Cause.Reconnection;

                            delete UCClient.SessionS[event.param.callid];
                            if (UCClient.callid == event.param.callid)
                                UCClient.callid = null;

                            if (typeof (UCClient.onCallReleased) == "function") {
                                UCClient.onCallReleased(event.param);
                            }
                        }
                        return;
                    case "onCallAnswered":
                        {
                            UCClient.SessionS[event.param.callid] = UCClient.SessionS[event.param.callid] || {};
                            if (UCClient.callid && UCClient.callid != event.param.callid) {
                                UCClient.SessionS[event.param.callid]._cause = UCClient.Cause.Consultation;
                            }
                            event.param.cause = UCClient.SessionS[event.param.callid]._cause;
                            UCClient.callid = event.param.callid;
                            if (typeof (UCClient.onCallAnswered) == "function") {
                                UCClient.onCallAnswered(event.param);
                            }
                        }
                        return;
                    case "onCallAlerting":
                        {
                            UCClient.SessionS[event.param.callid] = UCClient.SessionS[event.param.callid] || {};
                            if (UCClient.callid && UCClient.callid != event.param.callid) {
                                event.param.cause = UCClient.Cause.Consultation;
                                UCClient.SessionS[event.param.callid]._cause = UCClient.Cause.Consultation;
                            }
                            UCClient.callid = event.param.callid;
                            if (typeof (UCClient.onCallAlerting) == "function") {
                                UCClient.onCallAlerting(event.param);
                            }
                        }
                        return;
                    case "onIncomingCallReceived":
                        {
                            UCClient.callid = event.param.callid;
                            UCClient.SessionS[event.param.callid] = UCClient.SessionS[event.param.callid] || {};
                            if (typeof (UCClient.onIncomingCallReceived) == "function") {
                                UCClient.onIncomingCallReceived(event.param);
                            }
                        }
                        return;
                    case "onCallPaused":
                        {
                            if (UCClient.SessionS[event.param.callid]._status == UCClient.STATUS.STATUS_CONSULTATIONING) {
                                UCClient.setUserData(2, UCClient.SessionS[event.param.callid]._userdata);
                                UCClient.makeCall(UCClient.SessionS[event.param.callid]._consultNumber);
                                event.param.cause = UCClient.Cause.Consultation;
                            }

                            if (UCClient.SessionS[event.param.callid]._status == UCClient.STATUS.STATUS_ALTERNATEING)
                                event.param.cause = UCClient.Cause.Alternate;

                            if (typeof (UCClient.onCallPaused) == "function") {
                                UCClient.onCallPaused(event.param);
                            }
                        }
                        return;
                    case "onCallResumed":
                        {
                            UCClient.callid = event.param.callid;
                            if (UCClient.SessionS[event.param.callid]._status === UCClient.STATUS.STATUS_RECONNECTING)
                                UCClient.SessionS[event.param.callid]._status = UCClient.STATUS.STATUS_CONNECTED;

                            if (UCClient.SessionS[event.param.callid]._status == UCClient.STATUS.STATUS_ALTERNATEING)
                                event.param.cause = UCClient.Cause.Alternate;

                            if (typeof (UCClient.onCallResumed) == "function") {
                                UCClient.onCallResumed(event.param);
                            }
                        }
                        return;
                    case "onCallTransfered":
                        {
                            if (typeof (UCClient.onCallTransfered) == "function") {
                                UCClient.onCallTransfered(event.param);
                            }
                        }
                        return;
                    case "onDtmfReceived":
                        {
                            if (typeof (UCClient.onDtmfReceived) == "function") {
                                UCClient.onDtmfReceived(event.param);
                            }
                        }
                        return;
                    case "onMeetingTransfered":
                        {
                            if (typeof (UCClient.onMeetingTransfered) == "function") {
                                UCClient.onMeetingTransfered(event.param);
                            }
                        }
                        return;
                    default:
                        UCClient.warn("event未知事件:" + evt.data);
                        return;
                }
            }
            UCClient.warn("未知消息：" + evt.data);
        },

        wsonError: function (evt) {
            UCClient.error(evt.data || "连接失败");
            if (typeof (UCClient.onWSError) == "function") {
                UCClient.onWSError(evt.data || "连接失败");
            }
        },

        doSend: function (cmd) {
            var message = JSON.stringify(cmd)
            UCClient.debug("SENT: " + message);
            UCClient.websocket.send(message);
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
            UCClient.debug("Login,server:" + server + ":" + port + ",SipID:" + sipid + ",Pwd:" + pwd);

            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "connectToCCPServer";
            cmd.param = {};
            cmd.param.serverAddr = server;
            cmd.param.serverPort = parseInt(port);
            cmd.param.voipId = sipid;
            cmd.param.voipPwd = pwd;
            UCClient.doSend(cmd);
        },
        // 注销
        disConnectToCCP: function () {
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "disConnectToCCP";
            UCClient.doSend(cmd);
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
            UCClient.doSend(cmd);
        },

        //设置自定义数据
        setUserData: function (type, data) {
            UCClient.debug("setUserData: type:" + type + ",data:" + data);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "setUserData";
            cmd.param = {};
            cmd.param.type = type;
            cmd.param.data = data;
            UCClient.doSend(cmd);
        },
        //呼叫
        makeCall: function (called) {
            UCClient.debug("makeCall:" + called);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "makeCall";
            cmd.param = {};
            cmd.param.called = called;
            UCClient.doSend(cmd);
        },
        //摘机
        acceptCall: function (callid) {
            UCClient.debug("acceptCall:" + callid);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "acceptCall";
            cmd.param = {};
            cmd.param.callid = callid;
            UCClient.doSend(cmd);
        },

        // 挂断 (SIP BYE or CANCEL)
        releaseCall: function (callid, reason) {
            UCClient.debug("releaseCall, callid:" + callid + ",reason:" + reason);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "releaseCall";
            cmd.param = {};
            cmd.param.callid = callid;
            cmd.param.reason = reason;
            UCClient.doSend(cmd);
        },
        //发送DTMF
        sendDTMF: function (callid, c) {
            UCClient.debug("sendDTMF, callid:" + callid + ",dtmf:" + c);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "sendDTMF";
            cmd.param = {};
            cmd.param.callid = callid;
            cmd.param.dtmf = c;
            UCClient.doSend(cmd);
        },
        // 盲转
        transferCall: function (callid, s_destination) {
            UCClient.debug("transferCall,callid:" + callid + ",destination:" + s_destination);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "transferCall";
            cmd.param = {};
            cmd.param.callid = callid;
            cmd.param.dest = s_destination;
            cmd.param.type = 0;
            UCClient.doSend(cmd);
        },

        /**
         * 保持
         * @param callid
         * @returns {number}
         * @constructor
         */
        pauseCall: function (callid) {
            UCClient.debug("pauseCall,callid:" + callid);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "pauseCall";
            cmd.param = {};
            cmd.param.callid = callid;
            UCClient.doSend(cmd);
        },
        /**
         * 恢复
         * @param callid
         * @returns {number}
         * @constructor
         */
        resumeCall: function (callid) {
            UCClient.debug("resumeCall,callid:" + callid);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "resumeCall";
            cmd.param = {};
            cmd.param.callid = callid;
            UCClient.doSend(cmd);
        },

        //咨询
        ConsultationCall: function (callid, called, userdata) {
            UCClient.debug("ConsultationCall,callid:" + callid + ",called:" + called);
            if (callid && UCClient.SessionS[callid]) {
                UCClient.SessionS[callid]._status = UCClient.STATUS.STATUS_CONSULTATIONING;
                UCClient.SessionS[callid]._consultNumber = called;
                UCClient.SessionS[callid]._userdata = userdata;
                UCClient.pauseCall(callid);
                UCClient.debug('pauseCall the call...' + callid);
                //UCClient.MakeCall(called);                    
                return 0;

            } else {
                UCClient.error("ConsultationCall, the call is not exist.");
                return 3;
            }
        },

        /**
         * 取消咨询
         * @param callid
         * @constructor
         */
        ReconnectCall: function (activeCall, heldCall) {
            UCClient.debug("ReconnectCall,activeCall:" + activeCall + ",heldCall:" + heldCall);

            if (activeCall && UCClient.SessionS[activeCall]) {
                UCClient.SessionS[activeCall]._status = UCClient.STATUS.STATUS_RECONNECTING;
                UCClient.releaseCall(activeCall);
            }

            if (heldCall && UCClient.SessionS[heldCall]) {
                UCClient.SessionS[heldCall]._status = UCClient.STATUS.STATUS_RECONNECTING;
                UCClient.debug('resumeCall the call...' + heldCall);
                UCClient.resumeCall(heldCall);
            }
            else {
                UCClient.error("ReconnectCall, the call is not exist.");
            }

        },

        // 咨询后转接
        consultTransferCall:function(callid, consultCallid, destination){
            UCClient.debug("consultTransferCall,callid:" + callid + ", consultCallid:" + consultCallid + ",destination:" + destination);
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "consultTransferCall";
            cmd.param = {};
            cmd.param.callid = callid;
            cmd.param.consultCallid = consultCallid;
            cmd.param.dest = destination;
            cmd.param.type = 0;
            UCClient.doSend(cmd);
        },
        
        //切换通话
        AlternateCall: function (activeCall, otherCall) {
            UCClient.debug("AlternateCall,activeCall:" + activeCall + ",otherCall:" + otherCall);
            if (!activeCall || !UCClient.SessionS[activeCall]) {
                UCClient.warn("AlternateCall, the activeCall is not exist.");
            }
            else {
                UCClient.SessionS[activeCall]._status = UCClient.STATUS.STATUS_ALTERNATEING;
                UCClient.pauseCall(activeCall);
            }

            if (!otherCall || !UCClient.SessionS[otherCall]) {
                UCClient.warn("AlternateCall, the otherCall is not exist.");
            }
            else {
                UCClient.SessionS[otherCall]._status = UCClient.STATUS.STATUS_ALTERNATEING;
                UCClient.resumeCall(otherCall);
            }
        },

        //会议
        transferMeeting: function (type, otherCall, activeCall, consultedUser) {
            UCClient.debug("transferMeeting, type:" + type + ",activeCall:" + activeCall + ",otherCall:" + otherCall + ",consultedUser:" + consultedUser);
            if (activeCall && UCClient.SessionS[activeCall])
                UCClient.SessionS[activeCall]._status = UCClient.STATUS.STATUS_CONFERENCEING;

            if (otherCall && UCClient.SessionS[otherCall])
                UCClient.SessionS[otherCall]._status = UCClient.STATUS.STATUS_CONFERENCEING;
            
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "transferMeeting";
            cmd.param = {};
            cmd.param.callid = otherCall || activeCall;
            cmd.param.consultCallid = activeCall;
            cmd.param.consultedUser = consultedUser;
            cmd.param.type = type;
            UCClient.doSend(cmd);
        },


        //获取麦克风设备
        getMicroPhoneInfo: function () {

            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "getMicroPhoneInfo";
            //cmd.param = {};
            UCClient.doSend(cmd);
        },

        //设置麦克风设备
        selectMicroPhone: function (microId) {
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "selectMicroPhone";
            cmd.param = {};
            cmd.param.index = parseInt(microId);
            UCClient.doSend(cmd);
        },

        //获取扬声器设备
        getSpeakerInfo: function () {

            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "getSpeakerInfo";
            //cmd.param = {};
            UCClient.doSend(cmd);
        },

        //设置扬声器设备
        selectSpeaker: function (speakerId) {
            var cmd = {};
            cmd.type = "cmd";
            cmd.cmd = "selectSpeaker";
            cmd.param = {};
            cmd.param.index = parseInt(speakerId);
            UCClient.doSend(cmd);
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
            if (UCClient.loglevel == "trace") {
                c = "[" + UCClient.dateNow() + "] UCClient " + c;
                window.console.trace(c);
            }
        },

        debug: function (c) {
            if (UCClient.loglevel == "trace" || UCClient.loglevel == "debug") {
                c = "[" + UCClient.dateNow() + "] UCClient " + c;
                window.console.debug(c);
            }
        },


        info: function (c) {
            if (UCClient.loglevel == "trace" || UCClient.loglevel == "debug" || UCClient.loglevel == "info") {
                c = "[" + UCClient.dateNow() + "] UCClient " + c;
                window.console.info(c);
            }
        },

        warn: function (c) {
            if (UCClient.loglevel == "trace" || UCClient.loglevel == "debug" || UCClient.loglevel == "info" || UCClient.loglevel == "warn") {
                c = "[" + UCClient.dateNow() + "] UCClient " + c;
                window.console.warn(c);
            }
        },

        error: function (c) {
            if (UCClient.loglevel == "trace" || UCClient.loglevel == "debug" || UCClient.loglevel == "info" || UCClient.loglevel == "warn" || UCClient.loglevel == "error") {
                c = "[" + UCClient.dateNow() + "] UCClient " + c;
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