// Setup device publishing to '/device/status/{uuid}'
// Setup device subscribing to '/device/config/{uuid}'
// Setup device wifi connecting to broker

// Define variables: uuid, group_target (if applicable)

/*
    * define the callback
    * define two different scenarios in the callback
    *** /device/config/{target} (group)
    *** /device/config/{uuid} (device)
*/

// Loop device for publish on '/device/status/{uuid}'

#include <PubSubClient.h>
