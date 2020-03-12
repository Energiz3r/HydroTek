export const demoDevices = [
    {
        deviceId: 1,
        deviceShowing: false,
        deviceLabel: "The main device",
        loggingEnabled: true,
        uploadFrequency: 1,
        alertEmailAddress: 'tim.eastwood@hotmail.com',
        //singlePlantMode: false,
        devicePlants: [
            {
                plantId: 1,
                plantShowing: false,
                plantName: 'plant 1',

                tempEnabled: true,
                tempShowing: false,
                tempLampShutoff: false,
                tempLampMaxTemp: 0,
                tempHiEmail: false,
                tempHiAlarm: false,
                tempHiTemp: 35.5,
                tempLoEmail: false,
                tempLoAlarm: false,
                tempLoTemp: 35.5,

                lampEnabled: true,
                lampShowing: false,
                lampHeaterMode: false,
                lampHeaterTemp: 0,
                lampStartTimeV: "03:00",
                lampEndTimeV: "23:00",
                lampFlowerMode: false,
                lampStartTimeF: "07:00",
                lampEndTimeF: "21:00",
                lampInvertLogic: false,

                pumpEnabled: true,
                pumpShowing: false,
                pumpFlowMode: false,
                pumpFlowMl: 0,
                pumpFlowEmail: false,
                pumpFlowAlarm: false,
                pumpMaxDurationSec: 20,
                pumpFrequencyMins: 15,
                pumpInvertLogic: false,

                floatEnable: true,
                floatShowing: false,
                floatAlarm: false,
                floatPumpShutoff: true,
                floatInvertLogic: false,

                flowEnable: true,
                flowShowing: false

            },
            {
                plantId: 2,
                plantShowing: false,
                plantName: 'plant 2',

                tempEnabled: true,
                tempShowing: false,
                tempLampShutoff: false,
                tempLampMaxTemp: 0,
                tempHiEmail: false,
                tempHiAlarm: false,
                tempHiTemp: 35.5,
                tempLoEmail: false,
                tempLoAlarm: false,
                tempLoTemp: 35.5,

                lampEnabled: true,
                lampHeaterMode: false,
                lampHeaterTemp: 0,
                lampStartTimeV: "03:00",
                lampEndTimeV: "23:00",
                lampFlowerMode: false,
                lampStartTimeF: "07:00",
                lampEndTimeF: "21:00",
                lampInvertLogic: false,

                pumpEnabled: false,
                pumpFlowMode: false,
                pumpFlowMl: 0,
                pumpFlowEmail: false,
                pumpFlowAlarm: false,
                pumpMaxDurationSec: 20,
                pumpFrequencyMins: 15,
                pumpInvertLogic: false,

                floatEnable: false,
                floatAlarm: false,
                floatPumpShutoff: true,
                floatInvertLogic: false,

                flowEnable: false

            }
        ]
    },
    {
        deviceId: 2,
        deviceShowing: false,
        deviceLabel: "Back garden",
        loggingEnabled: true,
        uploadFrequency: 1,
        alertEmailAddress: 'tim.eastwood@hotmail.com',
        //singlePlantMode: false,
        devicePlants: [
            {
                plantId: 1,
                plantShowing: false,
                plantName: 'small shrub',

                tempEnabled: true,
                tempLampShutoff: false,
                tempLampMaxTemp: 0,
                tempHiEmail: false,
                tempHiAlarm: false,
                tempHiTemp: 35.5,
                tempLoEmail: false,
                tempLoAlarm: false,
                tempLoTemp: 35.5,

                lampEnabled: true,
                lampHeaterMode: false,
                lampHeaterTemp: 0,
                lampStartTimeV: "03:00",
                lampEndTimeV: "23:00",
                lampFlowerMode: false,
                lampStartTimeF: "07:00",
                lampEndTimeF: "21:00",
                lampInvertLogic: false,

                pumpEnabled: true,
                pumpFlowMode: false,
                pumpFlowMl: 0,
                pumpFlowEmail: false,
                pumpFlowAlarm: false,
                pumpMaxDurationSec: 20,
                pumpFrequencyMins: 15,
                pumpInvertLogic: false,

                floatEnable: true,
                floatAlarm: false,
                floatPumpShutoff: true,
                floatInvertLogic: false,

                flowEnable: true

            },
            {
                plantId: 2,
                plantShowing: false,
                plantName: 'tall tree',

                tempEnabled: true,
                tempLampShutoff: false,
                tempLampMaxTemp: 0,
                tempHiEmail: false,
                tempHiAlarm: false,
                tempHiTemp: 35.5,
                tempLoEmail: false,
                tempLoAlarm: false,
                tempLoTemp: 35.5,

                lampEnabled: true,
                lampHeaterMode: false,
                lampHeaterTemp: 0,
                lampStartTimeV: "03:00",
                lampEndTimeV: "23:00",
                lampFlowerMode: false,
                lampStartTimeF: "07:00",
                lampEndTimeF: "21:00",
                lampInvertLogic: false,

                pumpEnabled: false,
                pumpFlowMode: false,
                pumpFlowMl: 0,
                pumpFlowEmail: false,
                pumpFlowAlarm: false,
                pumpMaxDurationSec: 20,
                pumpFrequencyMins: 15,
                pumpInvertLogic: false,

                floatEnable: false,
                floatAlarm: false,
                floatPumpShutoff: true,
                floatInvertLogic: false,

                flowEnable: false

            }
        ]
    }
]