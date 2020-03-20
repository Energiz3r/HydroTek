import { connect } from 'react-redux'
import { setRoute } from '../actions/UIActions'
import { serverAPILocation } from '../config'
import ReactTooltip from "react-tooltip"
import { demoDevices } from "../utils/demo-device"
import { debounce } from 'lodash'

const serverAPILocation = serverAPILocation + 'api-device-config.php'

class Devices extends React.Component {
  constructor(props) {
    super(props)
    this.state = {
      deviceList: demoDevices,

      emailAlertsEnable: false,
      alertsEmailAddress: '',
      alertsEmailValid: false,

      waitingForSave: false,
      saveComplete: false
      
    }
  }
  onHomeClick = () => {
    this.props.dispatch(setRoute('home'))
  }
  componentDidMount = () => {
    this.setState({
      waitingForSave: true
    })
    fetch(serverAPILocation, {
      method: 'POST',
      cache: 'no-cache',
      headers: {
        'Content-Type': 'application/json'
      },
      body: JSON.stringify({
        "action": "getDeviceList"
      })
    })
      .then(result => result.json())
      .then(
        (result) => {
          if (result.deviceListAvailable === "1") {
            console.log(result)
            this.setState({
              waitingForSave: false,
              deviceList: result.deviceList,
              emailAlertsEnable: result.emailAlertsEnable,
              alertsEmailAddress: result.alertsEmailAddress
            })
          } else {
            console.log("Unable to fetch devices:")
            console.log(result)
          }
        },
        (error) => {
          console.log("Error:")
          console.log(error)
        }
      )
  }
  onDeviceAddClick = () => {
    //create a device on the server and fetch the config
  }
  onDeviceCaret = index => e => {
    const { deviceList } = this.state
    let x = deviceList[index.device]
    x.deviceShowing = !x.deviceShowing
    if (x.deviceShowing) {
      deviceList.map((device, i)=>{
        if (i != index.device) { deviceList[i].deviceShowing = false }
      })
    }
    this.setState({ ...this.state, deviceList })
  }
  onPlantCaret = index => e => {
    const { deviceList } = this.state
    let x = deviceList[index.device].devicePlants[index.plant]
    x.plantShowing = !x.plantShowing
    if (x.plantShowing) {
      deviceList[index.device].devicePlants.map((plant, i)=>{
        if (i != index.plant) {
          deviceList[index.device].devicePlants[i].plantShowing = false
        }
      })
    }
    this.setState({ ...this.state, deviceList })
  }
  onOptionGroupCaret = index => e => {
    e.stopPropagation()
    const { deviceList } = this.state
    const prevState = deviceList[index.device].devicePlants[index.plant].optionGroups[index.group].showing
    deviceList[index.device].devicePlants[index.plant].optionGroups.map((optGrp, i)=>{
      if (i == index.group) {
        optGrp.showing = !optGrp.showing
      } else if (!prevState) {
        optGrp.showing = false
      }
    })
    this.setState({ ...this.state, deviceList })
  }
  checkSaveValidation = () => {
    return  this.state.emailAlertsEnable && this.state.alertsEmailValid
            || !this.state.emailAlertsEnable
  }
  saveToServer = () => {
    if (this.checkSaveValidation()){
      this.setState({
        ...this.state,
        waitingForSave: true,
        saveComplete: false
      })
      console.log("Saving to server")

      //when save confirmed
      setTimeout(()=>{
        this.setState({
          ...this.state,
          waitingForSave: false,
          saveComplete: true
        })
        setTimeout(()=>{
          this.setState({
            ...this.state,
            saveComplete: false
          })
        }, 50)
      }, 2000)
    }
  }
  saveToServerOnClickDebounced = debounce(() => {
    this.saveToServer()
  }, 1000)
  saveToServerOnClick =  debounce(() => {
    this.saveToServer()
  }, 350)
  integerValidate = (value, max, min = 1) => {
    const intVal = parseInt(value) || 0
    return value == '' ? min :
      intVal > min ? Math.min(intVal, max) : min
  }
  floatValidate = (value, max, min = 1) => {
    const floatVal = parseFloat(value) || 0
    return value == '' ? min : 
      floatVal > min ? Math.min(floatVal, max) : min
      .toFixed(1) //1 dec place
  }
  textValidate = (value, maxLength = 255) => {
    return value.substr(0, maxLength)
  }
  emailValidate = (email) => {
    if (!email || email.match( /^(([^<>()[\]\\.,;:\s@\"]+(\.[^<>()[\]\\.,;:\s@\"]+)*)|(\".+\"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/ )) {
      return true
    } else return false
  }
  render() {
    const { deviceList } = this.state
    return (
      <div className="device-content">
        <h2>Cloud Configuration</h2>

        {!this.checkSaveValidation() && <h3 className="device-save-error">Can't save! Check errors in red below</h3>}

        <div className='device-save-icon-container'>
          {this.state.waitingForSave && <i className="fas fa-spinner fa-spin device-save-icon"></i>}
          <i className="fas fa-check device-save-icon device-save-complete-icon" style={
            this.state.saveComplete ? {
              'opacity': 1,
              '-webkit-transition': 'none',
              '-moz-transition': 'none',
              '-ms-transition': 'none',
              '-o-transition': 'none',
            } : {
              '-webkit-transition': 'opacity 2s ease-in-out',
              '-moz-transition': 'opacity 2s ease-in-out',
              '-ms-transition': 'opacity 2s ease-in-out',
              '-o-transition': 'opacity 2s ease-in-out',
              'opacity': 0
            }}></i>
        </div>

        {/* EMAIL ALERT SETTINGS */}
        <div className="device-container device-alerts-container">
          <div className="device-option-container">
            <label data-tip="Enable sending email alerts (for all devices)">Enable email alerts</label>
            {/* TOGGLE */}
            <div className={"switch-container" + (this.state.emailAlertsEnable ? " switch-container-on" : "")} onClick={()=>{
              this.setState({ ...this.state, emailAlertsEnable: !this.state.emailAlertsEnable })
              this.saveToServerOnClick()
            }}>
              <div className={"switch-slider" + (this.state.emailAlertsEnable ? " switch-slider-on" : "")}></div>
            </div>
          </div>
          <div className="device-option-container device-option-split-row">
            <label data-tip="Address to send device alerts to (if enabled)">Email address</label>
            <input className={"device-email-input" + (!this.state.alertsEmailValid && this.state.emailAlertsEnable ? ' device-invalid-email' : '')} type="text" placeholder="Email address" value={this.state.alertsEmailAddress}
            onChange={(e)=>{
              const email = e.target.value //  alertsEmailValid
              const valid = this.emailValidate(email)
              this.setState({
                ...this.state,
                alertsEmailAddress: email,
                alertsEmailValid: valid
              })
              if (valid) { this.saveToServerOnClickDebounced() }
            }}
            disabled={!this.state.emailAlertsEnable}></input>
          </div>
        </div>

        <h2>Device Configuration</h2>
          
        {deviceList.map((device, i)=> {
          return(
          <div className="device-container" key={i}>

            {/* DEVICE SETTINGS */}
            <div className="device-option-container device-label-container"
            onClick={this.onDeviceCaret({device: i})}>
              <i className={"fas fa-microchip device-icon" + (device.deviceShowing ? " icon-selected" : "")}></i>
              {/* TEXT */}
              <input className="device-label-input" type="text" placeholder="Device Label" data-tip="A friendly name to identify the device, eg. 'Front Room', or 'Left Tent'"
                value={device.deviceLabel} onClick={(e)=>{e.stopPropagation()}}
                onChange={(e)=>{
                  const { deviceList } = this.state
                  deviceList[i].deviceLabel = this.textValidate(e.target.value)
                  this.setState({ ...this.state, deviceList })
                  this.saveToServerOnClickDebounced()
                }} ></input>
              <i className={"fas fa-caret-right device-caret" + (device.deviceShowing ? " fa-rotate-90" : "")}></i>
            </div>
            {device.deviceShowing && <div>
            <div className="device-option-container">
              <label data-tip="Controls whether the device uploads sensor readings at all">Enable Online Logging</label>
              {/* TOGGLE */}
              <div className={"switch-container" + (device.loggingEnabled ? " switch-container-on" : "")}
                onClick={()=>{
                  const { deviceList } = this.state
                  deviceList[i].loggingEnabled = !deviceList[i].loggingEnabled
                  this.setState({ ...this.state, deviceList })
                  this.saveToServerOnClick()
                }}>
                <div className={"switch-slider" + (device.loggingEnabled ? " switch-slider-on" : "")}></div>
              </div>
            </div>
            <div className="device-option-container">
              <label data-tip="Frequency, in minutes, of how often to upload status to server">Upload frequency (mins)</label>
              <div className="device-number-group">
                {/* INTEGER */}
                <input type="number" className="device-number-input" value={device.uploadFrequency}
                  disabled={!device.loggingEnabled}
                  onChange={(e)=>{
                    const { deviceList } = this.state
                    deviceList[i].uploadFrequency = this.integerValidate(e.target.value, 60)
                    this.setState({ ...this.state, deviceList })
                    this.saveToServerOnClickDebounced()
                  }} ></input>
              </div>
            </div>

            {device.devicePlants.map((plant, j)=>{

              return (
              <div className='device-sub-container' key={j}
              onClick={this.onPlantCaret({device: i, plant: j})}>
                <div className="device-option-container device-label-container">
                  <i className={"fas fa-leaf device-icon" + (plant.plantShowing ? " icon-selected" : "")}></i>
                  {/* TEXT */}
                  <input className="device-label-input plant-name-input" type="text" data-tip="A friendly name for the plant being monitored by the device eg. 'Jalapenos'"
                    value={plant.plantName} onClick={(e)=>{e.stopPropagation()}}
                    onChange={(e)=>{
                      const { deviceList } = this.state
                      deviceList[i].devicePlants[j].plantName = this.textValidate(e.target.value)
                      this.setState({ ...this.state, deviceList })
                      this.saveToServerOnClickDebounced()
                    }} ></input>
                  <i className={"fas fa-caret-right device-caret" + (plant.plantShowing ? " fa-rotate-90" : "")}></i>
                </div>

                {plant.plantShowing && <div>

                {/* TEMP */}

                <div className="device-option-container device-label-container"
                onClick={this.onOptionGroupCaret({device: i, plant: j, group: 0})}>
                  <i className={"fas fa-thermometer-half device-icon" + (plant.optionGroups[0].showing ? " icon-selected" : "")}></i>
                  <p className='device-sub-title'>Temperature / Humidity Sensor</p>
                  <i className={"fas fa-caret-right device-caret" + (plant.optionGroups[0].showing ? " fa-rotate-90" : "")}></i>
                </div>
                {plant.optionGroups[0].showing && <div onClick={(e)=>{e.stopPropagation()}}>
                  <div className="device-option-container">
                    <label data-tip="Enable / Disable the temp / humidity sensor">Temp / humidity sensor enable</label>
                    {/* TOGGLE */}
                    <div className={"switch-container" + (plant.tempEnabled ? " switch-container-on" : "")} 
                      onClick={()=>{
                        const { deviceList } = this.state
                        deviceList[i].devicePlants[j].tempEnabled = !deviceList[i].devicePlants[j].tempEnabled
                        this.setState({ ...this.state, deviceList })
                        this.saveToServerOnClick()
                      }}>
                      <div className={"switch-slider" + (plant.tempEnabled ? " switch-slider-on" : "")}></div>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Turn off the lamp if this sensor reads too hot">Lamp over-temp shutoff</label>
                    {/* TOGGLE */}
                    <div className={"switch-container" + (plant.tempLampShutoff ? " switch-container-on" : "") + (!plant.tempEnabled || !plant.lampEnabled ? " switch-container-disabled" : "")}
                      onClick={!plant.tempEnabled || !plant.lampEnabled ?null:()=>{ /* if disabled */
                        const { deviceList } = this.state
                        deviceList[i].devicePlants[j].tempLampShutoff = !deviceList[i].devicePlants[j].tempLampShutoff
                        this.setState({ ...this.state, deviceList })
                        this.saveToServerOnClick()
                      }}>
                      <div className={"switch-slider" + (plant.tempLampShutoff ? " switch-slider-on" : "") + (!plant.tempEnabled || !plant.lampEnabled ? " switch-slider-disabled" : "")}></div>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label>Max lamp temp</label>
                    <div className="device-number-group">
                    {/* FLOAT */}
                    <input type="number" className="device-number-input" value={plant.tempLampMaxTemp}
                      disabled={!plant.tempEnabled || !plant.lampEnabled || !plant.tempLampShutoff}
                      onChange={(e)=>{
                        const { deviceList } = this.state
                        deviceList[i].devicePlants[j].tempLampMaxTemp = this.floatValidate(e.target.value, 60)
                        this.setState({ ...this.state, deviceList })
                        this.saveToServerOnClickDebounced()
                      }} ></input>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Send an email if this sensor reads too hot">Hi temp email alert</label>
                    {/* TOGGLE */}
                    <div className={"switch-container" + (plant.tempHiEmail ? " switch-container-on" : "") + (!plant.tempEnabled || !device.loggingEnabled ? " switch-container-disabled" : "")}
                      onClick={!plant.tempEnabled || !device.loggingEnabled ?null:()=>{ /* if disabled */
                        const { deviceList } = this.state
                        deviceList[i].devicePlants[j].tempHiEmail = !deviceList[i].devicePlants[j].tempHiEmail
                        this.setState({ ...this.state, deviceList })
                        this.saveToServerOnClick()
                      }}>
                      <div className={"switch-slider" + (plant.tempHiEmail ? " switch-slider-on" : "") + (!plant.tempEnabled || !device.loggingEnabled ? " switch-slider-disabled" : "")}></div>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Sound the buzzer on the device if this sensor reads too hot">Hi temp sound alarm</label>
                    {/* TOGGLE */}
                    <div className={"switch-container" + (plant.tempHiAlarm ? " switch-container-on" : "") + (!plant.tempEnabled ? " switch-container-disabled" : "")} 
                      onClick={!plant.tempEnabled ?null:()=>{ /* if disabled */
                        const { deviceList } = this.state
                        deviceList[i].devicePlants[j].tempHiAlarm = !deviceList[i].devicePlants[j].tempHiAlarm
                        this.setState({ ...this.state, deviceList })
                        this.saveToServerOnClick()
                      }}>
                      <div className={"switch-slider" + (plant.tempHiAlarm ? " switch-slider-on" : "") + (!plant.tempEnabled ? " switch-slider-disabled" : "")}></div>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label>Hi temp</label>
                    <div className="device-number-group">
                      {/* FLOAT */}
                      <input type="number" className="device-number-input" value={plant.tempHiTemp}
                        disabled={!plant.tempEnabled || (!plant.tempHiAlarm && !plant.tempHiEmail)}
                        onChange={(e)=>{
                          const { deviceList } = this.state
                          deviceList[i].devicePlants[j].tempHiTemp = this.floatValidate(e.target.value, 99)
                          this.setState({ ...this.state, deviceList })
                          this.saveToServerOnClickDebounced()
                        }} ></input>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Send an email if this sensor reads too cold">Lo temp email alert</label>
                    {/* TOGGLE */}
                    <div className={"switch-container" + (plant.tempLoEmail ? " switch-container-on" : "") + (!plant.tempEnabled || !device.loggingEnabled ? " switch-container-disabled" : "")}
                      onClick={!plant.tempEnabled || !device.loggingEnabled ?null:()=>{ /* if disabled */
                        const { deviceList } = this.state
                        deviceList[i].devicePlants[j].tempLoEmail = !deviceList[i].devicePlants[j].tempLoEmail
                        this.setState({ ...this.state, deviceList })
                        this.saveToServerOnClick()
                      }}>
                      <div className={"switch-slider" + (plant.tempLoEmail ? " switch-slider-on" : "") + (!plant.tempEnabled || !device.loggingEnabled ? " switch-slider-disabled" : "")}></div>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Sound the buzzer on the device if this sensor reads too cold">Lo temp sound alarm</label>
                    {/* TOGGLE */}
                    <div className={"switch-container" + (plant.tempLoAlarm ? " switch-container-on" : "") + (!plant.tempEnabled ? " switch-container-disabled" : "")}
                      onClick={!plant.tempEnabled ?null:()=>{ /* if disabled */
                        const { deviceList } = this.state
                        deviceList[i].devicePlants[j].tempLoAlarm = !deviceList[i].devicePlants[j].tempLoAlarm
                        this.setState({ ...this.state, deviceList })
                        this.saveToServerOnClick()
                      }}>
                      <div className={"switch-slider" + (plant.tempLoAlarm ? " switch-slider-on" : "") + (!plant.tempEnabled ? " switch-slider-disabled" : "")}></div>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label>Lo temp</label>
                    <div className="device-number-group">
                      {/* FLOAT */}
                      <input type="number" className="device-number-input" value={plant.tempLoTemp}
                        disabled={!plant.tempEnabled || (!plant.tempLoAlarm && !plant.tempLoEmail)}
                        onChange={(e)=>{
                          const { deviceList } = this.state
                          deviceList[i].devicePlants[j].tempLoTemp = this.floatValidate(e.target.value, 99)
                          this.setState({ ...this.state, deviceList })
                          this.saveToServerOnClickDebounced()
                        }} ></input>
                    </div>
                  </div>
                </div>}

                {/* LAMP  */}

                <div className="device-option-container device-label-container"
                onClick={this.onOptionGroupCaret({device: i, plant: j, group: 1})}>
                  <i className={"far fa-lightbulb device-icon" + (plant.optionGroups[1].showing ? " icon-selected" : "")}></i>
                  <p className='device-sub-title'>Grow Lamp</p>
                  <i className={"fas fa-caret-right device-caret" + (plant.optionGroups[1].showing ? " fa-rotate-90" : "")}></i>
                </div>
                {plant.optionGroups[1].showing && <div onClick={(e)=>{e.stopPropagation()}}>
                  <div className="device-option-container">
                    <label data-tip="Enable lamp control">Lamp enable</label>
                    {/* TOGGLE */}
                    <div className={"switch-container" + (plant.lampEnabled ? " switch-container-on" : "")}
                      onClick={()=>{
                        const { deviceList } = this.state
                        deviceList[i].devicePlants[j].lampEnabled = !deviceList[i].devicePlants[j].lampEnabled
                        this.setState({ ...this.state, deviceList })
                        this.saveToServerOnClick()
                      }}>
                      <div className={"switch-slider" + (plant.lampEnabled ? " switch-slider-on" : "")}></div>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Enable using the lamp as a heater">Heater mode</label>
                    {/* TOGGLE */}
                    <div className={"switch-container" + (plant.lampHeaterMode ? " switch-container-on" : "") + (!plant.lampEnabled || !plant.tempEnabled ? " switch-container-disabled" : "")}
                      onClick={!plant.lampEnabled || !plant.tempEnabled ?null:()=>{ /* if disabled */
                        const { deviceList } = this.state
                        deviceList[i].devicePlants[j].lampHeaterMode = !deviceList[i].devicePlants[j].lampHeaterMode
                        this.setState({ ...this.state, deviceList })
                        this.saveToServerOnClick()
                      }}>
                      <div className={"switch-slider" + (plant.lampHeaterMode ? " switch-slider-on" : "") + (!plant.lampEnabled || !plant.tempEnabled ? " switch-slider-disabled" : "")}></div>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label>Heater target temp</label>
                    <div className="device-number-group">
                      {/* FLOAT */}
                      <input type="number" className="device-number-input" value={plant.lampHeaterTemp}
                        disabled={!plant.lampEnabled || !plant.lampHeaterMode || !plant.tempEnabled}
                        onChange={(e)=>{
                          const { deviceList } = this.state
                          deviceList[i].devicePlants[j].lampHeaterTemp = this.floatValidate(e.target.value, 99)
                          this.setState({ ...this.state, deviceList })
                          this.saveToServerOnClickDebounced()
                        }} ></input>
                    </div>
                  </div>
                  <div className="device-option-container device-option-split-row">
                    <label data-tip="Start time when in Veg mode">Start time (veg)</label>
                    {/* TIME */}
                    <input className={"device-email-input" + (!plant.lampEnabled || plant.lampFlowerMode  ? " device-time-input-disabled" : "")}
                      type="time" value={plant.lampStartTimeV}
                      disabled={!plant.lampEnabled || plant.lampFlowerMode }
                      onChange={(e)=>{
                        const { deviceList } = this.state
                        deviceList[i].devicePlants[j].lampStartTimeV = e.target.value
                        this.setState({ ...this.state, deviceList })
                        this.saveToServerOnClickDebounced()
                      }}></input>
                  </div>
                  <div className="device-option-container device-option-split-row">
                    <label data-tip="End time when in Veg mode">End time (veg)</label>
                    {/* TIME */}
                    <input className={"device-email-input" + (!plant.lampEnabled || plant.lampFlowerMode  ? " device-time-input-disabled" : "")}
                      type="time" value={plant.lampEndTimeV}
                      disabled={!plant.lampEnabled || plant.lampFlowerMode }
                      onChange={(e)=>{
                        const { deviceList } = this.state
                        deviceList[i].devicePlants[j].lampEndTimeV = e.target.value
                        this.setState({ ...this.state, deviceList })
                        this.saveToServerOnClickDebounced()
                      }}></input>
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Enable using the flower hours">Plant is flowering</label>
                    {/* TOGGLE */}
                    <div className={"switch-container" + (plant.lampFlowerMode ? " switch-container-on" : "") + (!plant.lampEnabled ? " switch-container-disabled" : "")}
                      onClick={!plant.lampEnabled ?null:()=>{ /* if disabled */
                        const { deviceList } = this.state
                        deviceList[i].devicePlants[j].lampFlowerMode = !deviceList[i].devicePlants[j].lampFlowerMode
                        this.setState({ ...this.state, deviceList })
                        this.saveToServerOnClick()
                      }}>
                      <div className={"switch-slider" + (plant.lampFlowerMode ? " switch-slider-on" : "") + (!plant.lampEnabled ? " switch-slider-disabled" : "")}></div>
                    </div>
                  </div>
                  <div className="device-option-container device-option-split-row">
                    <label data-tip="Start time when in Flower mode">Start time (flower)</label>
                    {/* TIME */}
                    <input className={"device-email-input" + (!plant.lampEnabled || !plant.lampFlowerMode ? " device-time-input-disabled" : "")}
                      type="time" value={plant.lampStartTimeF}
                      disabled={!plant.lampEnabled || !plant.lampFlowerMode}
                      onChange={(e)=>{
                        const { deviceList } = this.state
                        deviceList[i].devicePlants[j].lampStartTimeF = e.target.value
                        this.setState({ ...this.state, deviceList })
                        this.saveToServerOnClickDebounced()
                      }}></input>
                  </div>
                  <div className="device-option-container device-option-split-row">
                    <label data-tip="End time when in Flower mode">End time (flower)</label>
                    {/* TIME */}
                    <input className={"device-email-input" + (!plant.lampEnabled || !plant.lampFlowerMode ? " device-time-input-disabled" : "")}
                      type="time" value={plant.lampEndTimeF}
                      disabled={!plant.lampEnabled || !plant.lampFlowerMode}
                      onChange={(e)=>{
                        const { deviceList } = this.state
                        deviceList[i].devicePlants[j].lampEndTimeF = e.target.value
                        this.setState({ ...this.state, deviceList })
                        this.saveToServerOnClickDebounced()
                      }}></input>
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Invert the relay logic">Invert lamp relay logic</label>
                    {/* TOGGLE */}
                    <div className={"switch-container" + (plant.lampInvertLogic ? " switch-container-on" : "") + (!plant.lampEnabled ? " switch-container-disabled" : "")}
                      onClick={!plant.lampEnabled ?null:()=>{ /* if disabled */
                        const { deviceList } = this.state
                        deviceList[i].devicePlants[j].lampInvertLogic = !deviceList[i].devicePlants[j].lampInvertLogic
                        this.setState({ ...this.state, deviceList })
                        this.saveToServerOnClick()
                      }}>
                      <div className={"switch-slider" + (plant.lampInvertLogic ? " switch-slider-on" : "") + (!plant.lampEnabled ? " switch-slider-disabled" : "")}></div>
                    </div>
                  </div>
                </div>}

                {/* PUMP */}

                <div className="device-option-container device-label-container"
                onClick={this.onOptionGroupCaret({device: i, plant: j, group: 2})}>
                  <i className={"fas fa-tint device-icon" + (plant.optionGroups[2].showing ? " icon-selected" : "")}></i>
                  <p className='device-sub-title'>Nutrient Pump</p>
                  <i className={"fas fa-caret-right device-caret" + (plant.optionGroups[2].showing ? " fa-rotate-90" : "")}></i>
                </div>
                {plant.optionGroups[2].showing && <div onClick={(e)=>{e.stopPropagation()}}>
                  <div className="device-option-container">
                    <label data-tip="Enable / disable the pump operation">Pump enabled</label>
                    {/* TOGGLE */}
                    <div className={"switch-container" + (plant.pumpEnabled ? " switch-container-on" : "")}
                      onClick={()=>{
                        const { deviceList } = this.state
                        deviceList[i].devicePlants[j].pumpEnabled = !deviceList[i].devicePlants[j].pumpEnabled
                        this.setState({ ...this.state, deviceList })
                        this.saveToServerOnClick()
                      }}>
                      <div className={"switch-slider" + (plant.pumpEnabled ? " switch-slider-on" : "")}></div>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Uses measured flow (mL) to control delivery of nutrient">Flow mode</label>
                    {/* TOGGLE */}
                    <div className={"switch-container" + (plant.pumpFlowMode ? " switch-container-on" : "") + (!plant.pumpEnabled ? " switch-container-disabled" : "")}
                      onClick={!plant.pumpEnabled ?null:()=>{ /* if disabled */
                        const { deviceList } = this.state
                        deviceList[i].devicePlants[j].pumpFlowMode = !deviceList[i].devicePlants[j].pumpFlowMode
                        this.setState({ ...this.state, deviceList })
                        this.saveToServerOnClick()
                      }}>
                      <div className={"switch-slider" + (plant.pumpFlowMode ? " switch-slider-on" : "") + (!plant.pumpEnabled ? " switch-slider-disabled" : "")}></div>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label>Nutrient volume per delivery (mL)</label>
                    <div className="device-number-group">
                      {/* INTEGER */}
                      <input type="number" className="device-number-input" value={plant.pumpFlowMl}
                        disabled={!plant.pumpEnabled || !plant.pumpFlowMode}
                        onChange={(e)=>{
                          const { deviceList } = this.state
                          deviceList[i].devicePlants[j].pumpFlowMl = this.integerValidate(e.target.value, 5999)
                          this.setState({ ...this.state, deviceList })
                          this.saveToServerOnClickDebounced()
                        }} ></input>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Alert by email if the flow volume can not be reached within the maximum time">Low-flow email</label>
                    {/* TOGGLE */}
                    <div className={"switch-container" + (plant.pumpFlowEmail ? " switch-container-on" : "") + (!plant.pumpEnabled || !plant.pumpFlowMode || !device.loggingEnabled  ? " switch-container-disabled" : "")}
                      onClick={!plant.pumpEnabled || !plant.pumpFlowMode || !device.loggingEnabled ?null:()=>{ /* if disabled */
                        const { deviceList } = this.state
                        deviceList[i].devicePlants[j].pumpFlowEmail = !deviceList[i].devicePlants[j].pumpFlowEmail
                        this.setState({ ...this.state, deviceList })
                        this.saveToServerOnClick()
                      }}>
                      <div className={"switch-slider" + (plant.pumpFlowEmail ? " switch-slider-on" : "") + (!plant.pumpEnabled || !plant.pumpFlowMode || !device.loggingEnabled  ? " switch-slider-disabled" : "")}></div>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Sound alarm if the flow volume can not be reached within the maximum time">Low-flow alarm</label>
                    {/* TOGGLE */}
                    <div className={"switch-container" + (plant.pumpFlowAlarm ? " switch-container-on" : "") + (!plant.pumpEnabled || !plant.pumpFlowMode  ? " switch-container-disabled" : "")}
                      onClick={!plant.pumpEnabled || !plant.pumpFlowMode ?null:()=>{ /* if disabled */
                        const { deviceList } = this.state
                        deviceList[i].devicePlants[j].pumpFlowAlarm = !deviceList[i].devicePlants[j].pumpFlowAlarm
                        this.setState({ ...this.state, deviceList })
                        this.saveToServerOnClick()
                      }}>
                      <div className={"switch-slider" + (plant.pumpFlowAlarm ? " switch-slider-on" : "") + (!plant.pumpEnabled || !plant.pumpFlowMode  ? " switch-slider-disabled" : "")}></div>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label>(Max) pumping duration (sec)</label>
                    <div className="device-number-group">
                      {/* INTEGER */}
                      <input type="number" className="device-number-input" value={plant.pumpMaxDurationSec}
                        disabled={!plant.pumpEnabled}
                        onChange={(e)=>{
                          const { deviceList } = this.state
                          deviceList[i].devicePlants[j].pumpMaxDurationSec = this.integerValidate(e.target.value, 599)
                          this.setState({ ...this.state, deviceList })
                          this.saveToServerOnClickDebounced()
                        }} ></input>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label>Pumping frequency (mins)</label>
                    <div className="device-number-group">
                      {/* INTEGER */}
                      <input type="number" className="device-number-input" value={plant.pumpFrequencyMins}
                        disabled={!plant.pumpEnabled}
                        onChange={(e)=>{
                          const { deviceList } = this.state
                          deviceList[i].devicePlants[j].pumpFrequencyMins = this.integerValidate(e.target.value, 599)
                          this.setState({ ...this.state, deviceList })
                          this.saveToServerOnClickDebounced()
                        }} ></input>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Invert relay logic">Invert pump relay logic</label>
                    {/* TOGGLE */}
                    <div className={"switch-container" + (plant.pumpInvertLogic ? " switch-container-on" : "") + (!plant.pumpEnabled ? " switch-container-disabled" : "")}
                      onClick={!plant.pumpEnabled ?null:()=>{ /* if disabled */
                        const { deviceList } = this.state
                        deviceList[i].devicePlants[j].pumpInvertLogic = !deviceList[i].devicePlants[j].pumpInvertLogic
                        this.setState({ ...this.state, deviceList })
                        this.saveToServerOnClick()
                      }}>
                      <div className={"switch-slider" + (plant.pumpInvertLogic ? " switch-slider-on" : "") + (!plant.pumpEnabled ? " switch-slider-disabled" : "")}></div>
                    </div>
                  </div>
                </div>}

                {/* FLOAT */}

                <div className="device-option-container device-label-container"
                onClick={this.onOptionGroupCaret({device: i, plant: j, group: 3})}>
                  <i className={"fas fa-fill-drip device-icon" + (plant.optionGroups[3].showing ? " icon-selected" : "")}></i>
                  <p className='device-sub-title'>Float Sensor</p>
                  <i className={"fas fa-caret-right device-caret" + (plant.optionGroups[3].showing ? " fa-rotate-90" : "")}></i>
                </div>
                {plant.optionGroups[3].showing && <div onClick={(e)=>{e.stopPropagation()}}>
                  <div className="device-option-container">
                    <label data-tip="Enable / disable the sensor operation">Enable float sensor</label>
                    {/* TOGGLE */}
                    <div className={"switch-container" + (plant.floatEnable ? " switch-container-on" : "")}
                      onClick={()=>{
                        const { deviceList } = this.state
                        deviceList[i].devicePlants[j].floatEnable = !deviceList[i].devicePlants[j].floatEnable
                        this.setState({ ...this.state, deviceList })
                        this.saveToServerOnClick()
                      }}>
                      <div className={"switch-slider" + (plant.floatEnable ? " switch-slider-on" : "")}></div>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Sound alarm when triggered">Alarm when triggered</label>
                    {/* TOGGLE */}
                    <div className={"switch-container" + (plant.floatAlarm ? " switch-container-on" : "") + (!plant.floatEnable ? " switch-container-disabled" : "")}
                      onClick={!plant.floatEnable ?null:()=>{ /* if disabled */
                        const { deviceList } = this.state
                        deviceList[i].devicePlants[j].floatAlarm = !deviceList[i].devicePlants[j].floatAlarm
                        this.setState({ ...this.state, deviceList })
                        this.saveToServerOnClick()
                      }}>
                      <div className={"switch-slider" + (plant.floatAlarm ? " switch-slider-on" : "") + (!plant.floatEnable ? " switch-slider-disabled" : "")}></div>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Send email when triggered">Email when triggered</label>
                    {/* TOGGLE */}
                    <div className={"switch-container" + (plant.floatEmail ? " switch-container-on" : "") + (!plant.floatEnable || !device.loggingEnabled ? " switch-container-disabled" : "")}
                      onClick={!plant.floatEnable || !device.loggingEnabled ?null:()=>{ /* if disabled */
                        const { deviceList } = this.state
                        deviceList[i].devicePlants[j].floatEmail = !deviceList[i].devicePlants[j].floatEmail
                        this.setState({ ...this.state, deviceList })
                        this.saveToServerOnClick()
                      }}>
                      <div className={"switch-slider" + (plant.floatEmail ? " switch-slider-on" : "") + (!plant.floatEnable || !device.loggingEnabled ? " switch-slider-disabled" : "")}></div>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Prevent the pump operation when triggered">Pump shutoff</label>
                    {/* TOGGLE */}
                    <div className={"switch-container" + (plant.floatPumpShutoff ? " switch-container-on" : "") + (!plant.floatEnable || !plant.pumpEnabled ? " switch-container-disabled" : "")}
                      onClick={!plant.floatEnable || !plant.pumpEnabled ?null:()=>{ /* if disabled */
                        const { deviceList } = this.state
                        deviceList[i].devicePlants[j].floatPumpShutoff = !deviceList[i].devicePlants[j].floatPumpShutoff
                        this.setState({ ...this.state, deviceList })
                        this.saveToServerOnClick()
                      }}>
                      <div className={"switch-slider" + (plant.floatPumpShutoff ? " switch-slider-on" : "") + (!plant.floatEnable || !plant.pumpEnabled ? " switch-slider-disabled" : "")}></div>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Invert sensor logic">Invert sensor logic</label>
                    {/* TOGGLE */}
                    <div className={"switch-container" + (plant.floatInvertLogic ? " switch-container-on" : "") + (!plant.floatEnable ? " switch-container-disabled" : "")}
                      onClick={!plant.floatEnable ?null:()=>{ /* if disabled */
                        const { deviceList } = this.state
                        deviceList[i].devicePlants[j].floatInvertLogic = !deviceList[i].devicePlants[j].floatInvertLogic
                        this.setState({ ...this.state, deviceList })
                        this.saveToServerOnClick()
                      }}>
                      <div className={"switch-slider" + (plant.floatInvertLogic ? " switch-slider-on" : "") + (!plant.floatEnable ? " switch-slider-disabled" : "")}></div>
                    </div>
                  </div>
                </div>}

                {/* FLOW */}

                <div className="device-option-container device-label-container"
                onClick={this.onOptionGroupCaret({device: i, plant: j, group: 4})}>
                  <i className={"fas fa-water device-icon" + (plant.optionGroups[4].showing ? " icon-selected" : "")}></i>
                  <p className='device-sub-title'>Flow Sensor</p>
                  <i className={"fas fa-caret-right device-caret" + (plant.optionGroups[4].showing ? " fa-rotate-90" : "")}></i>
                </div>
                {plant.optionGroups[4].showing && <div onClick={(e)=>{e.stopPropagation()}}>
                  <div className="device-option-container">
                    <label data-tip="Enable flow sensor">Enable flow sensor</label>
                    {/* TOGGLE */}
                    <div className={"switch-container" + (plant.flowEnable ? " switch-container-on" : "")} onClick={()=>{
                      const { deviceList } = this.state
                      deviceList[i].devicePlants[j].flowEnable = !deviceList[i].devicePlants[j].flowEnable
                      this.setState({ ...this.state, deviceList })
                      this.saveToServerOnClick()
                    }}>
                      <div className={"switch-slider" + (plant.flowEnable ? " switch-slider-on" : "")}></div>
                    </div>
                  </div>
                </div>}

                </div>}

              </div>)

            })}
            </div>}

          </div>
          )
        })}

        <ReactTooltip place="top" type="dark" effect="solid" />

        <button className='button-default' onClick={this.onDeviceAddClick}>Add Device</button>

      </div>
    )
  }
}

export default connect(state=>state)(Devices)