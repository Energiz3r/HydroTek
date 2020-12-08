export const demoDevices = [
    {
        deviceLabel: "Big Tent",
        loggingEnabled: true,
        uploadFrequency: 5,
        devicePlants: [
            {
                plantName: 'Strawberries',

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
                lampStartTimeV: "05:00",
                lampEndTimeV: "23:00",
                lampFlowerMode: false,
                lampStartTimeF: "08:00",
                lampEndTimeF: "20:00",
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
                floatEmail: false,
                floatPumpShutoff: true,
                floatInvertLogic: false,

                flowEnable: true
            },
            {
                plantName: 'Tomatoes',

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
                lampStartTimeV: "05:00",
                lampEndTimeV: "23:00",
                lampFlowerMode: false,
                lampStartTimeF: "08:00",
                lampEndTimeF: "20:00",
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
                floatEmail: false,
                floatPumpShutoff: true,
                floatInvertLogic: false,

                flowEnable: false

            }
        ]
    },
    {
        deviceLabel: "Small Tent",
        loggingEnabled: true,
        uploadFrequency: 1,
        devicePlants: [
            {
                plantName: 'Chilis',

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
                lampStartTimeV: "05:00",
                lampEndTimeV: "23:00",
                lampFlowerMode: false,
                lampStartTimeF: "08:00",
                lampEndTimeF: "20:00",
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
                floatEmail: false,
                floatPumpShutoff: true,
                floatInvertLogic: false,

                flowEnable: true
            }
        ]
    }
]

demoDevices.map((device)=>{
    device.deviceShowing = false
    device.devicePlants.map((plant)=>{
        plant.plantShowing = false
        plant.optionGroups = [
            { showing: false },
            { showing: false },
            { showing: false },
            { showing: false },
            { showing: false }
        ]
    })
})