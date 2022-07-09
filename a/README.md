# iot_architecture *A*
Reference: https://www.mongodb.com/cloud-explained/iot-architecture

Devices: This stage is about the actual devices in the IoT solutions. These devices
could be sensors or actuators in the Perception layer. Those devices will generate
data (in the case of sensors) or act on their environment (in the case of actuators).
The data produced is converted in a digital form and transmitted to the internet
gateway stage. Unless a critical decision must be made, the data is typically sent in
a raw state to the next stage due to the limited resources of the devices
themselves.

Internet gateways: The internet gateway stage will receive the raw data from the
devices and pre-process it before sending it to the cloud. This internet gateway
could be physically attached to the device or a stand-alone device that could
communicate with sensors over low power networks and relay the data to the
internet.

Edge or fog computing: In order to process data as quickly as possible, you
might want to send your data to the edge of the cloud. This will let you analyze the
data quickly and identify if something requires immediate attention. This layer
typically would only be concerned with recent data that is required for time-critical
operations. Some pre-processing might be done at this stage, too, to limit the data
that is ultimately transferred to the cloud.

Cloud or data center: In this final stage, the data is stored for later processing.
The application and business layers live in this stage, where dashboards or
management software can be fed through the data stored in the cloud. Deep
analysis or resource-intensive operations such as machine learning training will
happen at this stage.
