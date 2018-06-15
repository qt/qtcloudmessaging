QT CLOUD MESSAGING API INSTALLATION:

1. To install with available backend add following to qmake script:
    qmake "CONFIG += embedded-kaltiot"
    or
    qmake "CONFIG += firebase"

    e.g.:
    qmake "CONFIG += embedded-kaltiot firebase"
    make qmake_all
    make
    make install

Google Firebase requirements:
2. Download and unzip google firebase c++ SDK:
    https://firebase.google.com/docs/cpp/setup

3. To use firebase as backend, define the following ENVIRONMENT variable
    GOOGLE_FIREBASE_SDK =
    and make it to point to your firebase sdk root

Embedded-Kaltiot requirements:
4. To use in embedded Kaltiot backend:
    3.1. Register your app in
        https://console.torqhub.io/signin

    3.2. Download platform SDK from the console download page

    3.3. Add environment variable to pointing to downloaded and unzipped SDK root:
        KALTIOT_SDK = ../../../<yourappname>_RasperryPi_SDK_1.0.17


5. Install first the QtCloudMessaging from command line with:

    qmake "CONFIG += embedded-kaltiot firebase"
    make
    make install

6. Now you can use the QtCloudMessaging in your app with

    Just API wrapper (e.g. creating new service providers)
    QT += cloudmessaging

    With Firebase backend:
    QT += cloudmessagingfirebase

    With Embedded devices & Kaltiot
    QT += cloudmessagingembeddedkaltiot

    See more from the example apps.
    NOTE: Examples are in Github:
    https://github.com/snowgrains/qtcloudmessaging-examples

7. Usage basics:

    main.cpp:

    // Using QCloudMessaging:
    #include <QtCloudMessaging>
    #include <kaltiotdemo.h>

    // Depending on your backend provider, add:

    // For Embedded systems
    #include <QtCloudMessagingEmbeddedKaltiot>

    // for Google firebase
    #include <QCloudMessagingFirebaseProvider>

    #include <QGuiApplication>
    #include <QQmlApplicationEngine>
    #include <QQmlContext>

    int main(int argc, char *argv[])
    {
        QGuiApplication app(argc, argv);

        QQmlApplicationEngine engine;

        //*** QTCLOUDMSG DEFINITIONS
        QCloudMessaging *pushServices = new QCloudMessaging();

        //***** For Kaltiot Embedded systems
        QCloudMessagingEmbeddedKaltiotProvider *kaltiotPushService = new QCloudMessagingEmbeddedKaltiotProvider()

        // Provider based init parameters are given with QVariantMap
        QVariantMap provider_params;
        provider_params["API_KEY"] = "Your API key from the Kaltiot console for server communication";

        // Creating name for provider which can be used cross your app.
        pushServices->registerProvider("KaltiotService", kaltiotPushService, provider_params);

        QVariantMap client_params;
        client_params["address"] = "IOTSensor1";
        client_params["version"] = "1.0";
        client_params["customer_id"] = "Kaltiot";
        clientParams[QStringLiteral("kaltiot_daemon_path")] = QStringLiteral("127.0.0.1:50000");

        QVariantList channels;
        channels.append("Temperatures");
        client_params["channels"] = channels;

        /*! Connected client for the device.
          \param Service name "KaltiotService"
          \param Client identifier name to be used inside the application
          \param Parameters for the client specific for the provider API
        */
        pushServices->connectClient("KaltiotService", "IOTSensor1", client_params);

        //***** OR For Firebase mobile systems:
        QCloudMessagingFirebaseProvider *m_firebaseService = new QCloudMessagingFirebaseProvider();

        QVariantMap provider_params;

        // Server API key is not recommended to store inside to the application code due security reasons.
        // But if you do, make sure it is inside compiled C file or if you are doing a server side implementation with C++ & Qt.
        // SERVER_API_KEY Is needed to be able to send topic messages from the client without Firebase application server.

        provider_params["SERVER_API_KEY"] = "Get your SERVER API KEY from the google firebase console";

        // Registering the Google firebase service component.
        pushServices->registerProvider("GoogleFireBase", m_firebaseService, provider_params);

        /*! Connected client is needed for mobile device.
          \param Service name "GoogleFireBase"
          \param Client identifier name to be used inside the demo application
          \param Parameters for the client. No params for firebase client.
        */
        pushServices->connectClient("GoogleFireBase", "MobileClient", QVariantMap());

        //! Automatically subscribe to listen one example topic
        pushServices->subscribeToChannel("ChatRoom", "GoogleFireBase", "MobileClient");

        //*** END OF QTCLOUD MSG DEFINITIONS

        // these are needed for keeping the received RID in memory after restart (in Android)
        QCoreApplication::setOrganizationName("MyOrganisation");
        QCoreApplication::setOrganizationDomain("MyOrganisation.com");
        QCoreApplication::setApplicationName("QtCloudMessagingDemo");


        // To Give QML the push service context:
        engine.rootContext()->setContextProperty("pushServices", pushServices);

        engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

        return app.exec();
    }

    main.qml:

    import QtQuick 2.6
    import QtQuick.Window 2.2


    Window {
        visible: true
        width: 640
        height: 480
        title: qsTr("Qt Cloud Messaging Demo")


        // NOTIFICATIONS FROM THE ALL PROVIDERS:
        Connections {
           target:pushServices
           onMessageReceived:{
                console.log("Message to " + providerId + " service to " + clientId + " client.")
                console.log("Message: " + message)

                var msg_in_json = JSON.parse(decodeURI(message));

                // Example to respond to embedded system request:
                if (msg_in_json.command === "REQUESTING_TEMPERATURE")
                    embeddedPublishTemperatureToServer(msg_in_json.serverID, mydevicecommand.getTemperature());

                // Or firebase the message itself is a container of the info.
                updateGameNotification(message);
           }

           // Own Uuid to be used or broadcasted to server.
           onClientTokenReceived: {

                console.log("MY Uuid:"+rid)

                // Id this is server code:
                serverUuid = rid;

                // Id this is client code:
                clientUuid = rid;

           }
        }
        Component.onCompleted {
            // In case android app and firebase sdk gets started fast and token is needed, it can be requested with:
            clientUuid = pushServices.clientToken("GoogleFireBase", "MobileClient");
        }

        //***** EMBEDDED DEVICES

        // Function to send msg as server with Kaltiot provider to all clients as broadcast
        property string serverUuid: ""

        function sendKaltiotEmbeddedServerMessage(command_msg, msg) {

            var payload = {command: command_msg, server_id : serverUuid}; // e.g. "REQUESTING_TEMPERATURE"
            var payload_array = [{"payload_type": "STRING","payload": encodeURI(JSON.stringify(payload))}]
            var p = "payload=" + JSON.stringify( payload_array );

            pushServices.sendMessage(p, "KaltiotService", "", "", "Temperatures");
        }

        // Function to send temperature status message from the embedded client to Kaltiot server:
        property string clientUuid :""

        function embeddedPublishTemperatureToServer(serverUuid, temperature) {
            var payload = {command: "TEMPERATURE_INFO", clientId: clientUuid}
            var payload_array = [{"payload_type": "STRING","payload": encodeURI(JSON.stringify(payload))}]
            var p = "payload=" + JSON.stringify( payload_array );

            pushServices.sendMessage(p, "KaltiotService", "IOTSensor1", serverUuid,"");
        }
        //***** FIREBASE
        // Function to post msg as a server and broadcast it to all clients in the same room
        function sendFirebaseServerMessage(msg){
            var data = {"data": {"message": {"text": msg } },
                        "notification": {"body": msg, "title": "Qt Cloud Messaging Chat"}}

            /*!
             * \brief SendMessage
             * \param msg               Message as text which needs to be modified to the provider specific JSON.
             * \param providerId        Provider name specified at config
             * \param localclientId     clientId spefied at startup
             * \param send_to_device    Device Uuid (rid) to send for one known device
             * \param send_to_channel   Channel name to broadcast message to channel
             * \return
             */

            pushServices.sendMessage(JSON.stringify(data), "GoogleFireBase", "MobileClient", "", "ChatRoom");
        }
    }



