import { connect } from 'react-redux'
import { setRoute } from '../actions/UIActions'
import { serverAPILocation } from '../config'
import Toggle from './Toggle'
import ReactTooltip from "react-tooltip"
import { demoDevices } from "../utils/demo-device"
import { debounce } from 'lodash'

class Devices extends React.Component {
  constructor(props) {
    super(props)
    this.state = {
      deviceList: demoDevices,
      emailAlertsEnable: false,
      alertsEmailAddress: '',
      alertsEmailValid: false,
      waitingForAdd: false,
      loadingDevices: true
    }
  }
  componentDidMount = () => {
    setInterval(()=>{
      this.state.deviceList[0].loggingEnabled = !this.state.deviceList[0].loggingEnabled
      this.forceUpdate()
    }, 500)
  }
  onHomeClick = () => {
    this.props.dispatch(setRoute('/home'))
  }
  loadDeviceList = () => {
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
            //handle response
          } else {
            console.log("Failed create account:")
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
    return this.state.emailAlertsEnable && this.state.alertsEmailValid || !this.state.emailAlertsEnable
  }
  saveToServerDebounced = debounce(() => {
    if (this.checkSaveValidation()){
      console.log("save to server")
    }    
  }, 1000)
  saveToServer =  debounce(() => {
    if (this.checkSaveValidation()){
      console.log("save to server")
    }    
  }, 350)
  integerValidate = (value, max, min = 1) => {
    const intVal = parseInt(value) || 0
    return value == '' ? '' :
      intVal > min ? Math.min(intVal, max) : min
  }
  floatValidate = (value, max, min = 1) => {
    const floatVal = parseFloat(value) || 0
    return value == '' ? '' : 
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
        <h2>Device Configuration</h2>

        {!this.checkSaveValidation() && <h3 className="device-save-error">Can't save! Check errors in red below!</h3>}

        {/* EMAIL ALERT SETTINGS */}
        <div className="device-container device-alerts-container">
          <div className="device-option-container">
            <label data-tip="Enable sending email alerts (for all devices)">Enable email alerts</label>
            <Toggle isChecked={this.state.emailAlertsEnable} onChange={()=>{
              this.setState({ ...this.state, emailAlertsEnable: !this.state.emailAlertsEnable })
              this.saveToServer()
            }} />
          </div>
          <div className="device-option-container device-option-split-row">
            <label data-tip="Address to send device alerts to (if enabled)">Alert email address (all devices)</label>
            <input className={"device-email-input" + (!this.state.alertsEmailValid && this.state.emailAlertsEnable ? ' device-invalid-email' : '')} type="text" placeholder="Email address" value={this.state.alertsEmailAddress}
            onChange={(e)=>{
              const email = e.target.value //  alertsEmailValid
              const valid = this.emailValidate(email)
              this.setState({
                ...this.state,
                alertsEmailAddress: email,
                alertsEmailValid: valid
              })
              if (valid) { this.saveToServerDebounced() }
            }}
            disabled={!this.state.emailAlertsEnable}></input>
          </div>
        </div>
          
        {deviceList.map((device, i)=> {
          return(
          <div className="device-container" key={i}>

            {/* DEVICE SETTINGS */}
            <div className="device-option-container device-label-container"
            onClick={this.onDeviceCaret({device: i})}>
              <i className={"fas fa-microchip device-icon" + (device.deviceShowing ? " icon-selected" : "")}></i>
              <input className="device-label-input" type="text" placeholder="Device Label" data-tip="A friendly name to identify the device, eg. 'Front Room', or 'Left Tent'"
                value={device.deviceLabel} onClick={(e)=>{e.stopPropagation()}}
                onChange={(e)=>{
                  const { deviceList } = this.state
                  deviceList[i].deviceLabel = this.textValidate(e.target.value)
                  this.setState({ ...this.state, deviceList })
                  this.saveToServerDebounced()
                }} ></input>
              <i className={"fas fa-caret-right device-caret" + (device.deviceShowing ? " fa-rotate-90" : "")}></i>
            </div>
            {device.deviceShowing && <div>
            <div className="device-option-container">
              <label data-tip="Controls whether the device uploads sensor readings at all">Enable Online Logging</label>
              <Toggle isChecked={device.loggingEnabled} onChange={()=>{
                const { deviceList } = this.state
                deviceList[i].loggingEnabled = !deviceList[i].loggingEnabled
                this.setState({ ...this.state, deviceList })
                this.saveToServer()
              }} />
              <p>{device.loggingEnabled ? "checked" : "unchecked"}</p>
            </div>
            <div className="device-option-container">
              <label data-tip="Frequency, in minutes, of how often to upload status to server">Upload frequency (mins)</label>
              <div className="device-number-group">
                <input type="number" className="device-number-input" value={device.uploadFrequency} disabled={device.loggingEnabled}
                onChange={(e)=>{
                  const { deviceList } = this.state
                  deviceList[i].uploadFrequency = this.integerValidate(e.target.value, 60)
                  this.setState({ ...this.state, deviceList })
                  this.saveToServerDebounced()
                }} ></input>
              </div>
            </div>

            {device.devicePlants.map((plant, j)=>{

              return (
              <div className='device-sub-container' key={j}
              onClick={this.onPlantCaret({device: i, plant: j})}>
                <div className="device-option-container device-label-container">
                  <i className={"fas fa-leaf device-icon" + (plant.plantShowing ? " icon-selected" : "")}></i>
                  <input className="device-label-input plant-name-input" type="text" data-tip="A friendly name for the plant being monitored by the device eg. 'Jalapenos'"
                    value={plant.plantName} onClick={(e)=>{e.stopPropagation()}}
                    onChange={(e)=>{
                      const { deviceList } = this.state
                      deviceList[i].devicePlants[j].plantName = this.textValidate(e.target.value)
                      this.setState({ ...this.state, deviceList })
                      this.saveToServerDebounced()
                    }} ></input>
                  <i className={"fas fa-caret-right device-caret" + (plant.plantShowing ? " fa-rotate-90" : "")}></i>
                </div>

                {plant.plantShowing && <div>

                {/* TEMP */}

                <div className="device-option-container device-label-container"
                onClick={this.onOptionGroupCaret({device: i, plant: j, group: 0})}>
                  <i className={"fas fa-thermometer-half device-icon" + (plant.optionGroups[0].showing ? " icon-selected" : "")}></i>
                  <p className='device-sub-title'>Temperature / Humidity</p>
                  <i className={"fas fa-caret-right device-caret" + (plant.optionGroups[0].showing ? " fa-rotate-90" : "")}></i>
                </div>
                {plant.optionGroups[0].showing && <div onClick={(e)=>{e.stopPropagation()}}>
                  <div className="device-option-container">
                    <label data-tip="Enable / Disable the temp / humidity sensor">Temp / humidity sensor enable</label>
                    <Toggle isChecked={plant.tempEnabled} onChange={()=>{
                      const { deviceList } = this.state
                      deviceList[i].devicePlants[j].tempEnabled = !deviceList[i].devicePlants[j].tempEnabled
                      this.setState({ ...this.state, deviceList })
                      this.saveToServer()
                    }} />
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Turn off the lamp if this sensor reads too hot">Lamp over-temp shutoff</label>
                    <Toggle isChecked={plant.tempLampShutoff} />
                  </div>
                  <div className="device-option-container">
                    <label>Max lamp temp</label>
                    <div className="device-number-group">
                      <input type="number" className="device-number-input" defaultValue="1"></input>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Send an email if this sensor reads too hot">Hi temp email alert</label>
                    <Toggle isChecked={true} />
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Sound the buzzer on the device if this sensor reads too hot">Hi temp sound alarm</label>
                    <Toggle isChecked={true} />
                  </div>
                  <div className="device-option-container">
                    <label>Hi temp</label>
                    <div className="device-number-group">
                      <input type="number" className="device-number-input" defaultValue="35.5"></input>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Send an email if this sensor reads too cold">Lo temp email alert</label>
                    <Toggle isChecked={true} />
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Sound the buzzer on the device if this sensor reads too cold">Lo temp sound alarm</label>
                    <Toggle isChecked={true} />
                  </div>
                  <div className="device-option-container">
                    <label>Lo temp</label>
                    <div className="device-number-group">
                      <input type="number" className="device-number-input" defaultValue="15.5"></input>
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
                    <Toggle isChecked={true} />
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Enable using the lamp as a heater">Heater mode</label>
                    <Toggle isChecked={true} />
                  </div>
                  <div className="device-option-container">
                    <label>Heater target temp</label>
                    <div className="device-number-group">
                      <input type="number" className="device-number-input" defaultValue="0"></input>
                    </div>
                  </div>
                  <div className="device-option-container device-option-split-row">
                    <label data-tip="Start time when in Veg mode">Start time (veg)</label>
                    <input className="device-email-input" type="text" placeholder="03:00"></input>
                  </div>
                  <div className="device-option-container device-option-split-row">
                    <label data-tip="End time when in Veg mode">End time (veg)</label>
                    <input className="device-email-input" type="text" placeholder="23:00"></input>
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Enable using the flower hours">Plant flowering</label>
                    <Toggle isChecked={true} />
                  </div>
                  <div className="device-option-container device-option-split-row">
                    <label data-tip="Start time when in Flower mode">Start time (flower)</label>
                    <input className="device-email-input" type="text" placeholder="07:00"></input>
                  </div>
                  <div className="device-option-container device-option-split-row">
                    <label data-tip="End time when in Flower mode">End time (flower)</label>
                    <input className="device-email-input" type="text" placeholder="21:00"></input>
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Invert the relay logic">Invert lamp relay logic</label>
                    <Toggle isChecked={true} />
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
                    <label data-tip="Uses measured flow (mL) to control delivery of nutrient">Flow mode</label>
                    <Toggle isChecked={true} />
                  </div>
                  <div className="device-option-container">
                    <label>Nutrient volume per delivery (mL)</label>
                    <div className="device-number-group">
                      <input type="number" className="device-number-input" defaultValue="35"></input>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Alert by email if the flow volume can not be reached within the maximum time">Low-flow email</label>
                    <Toggle isChecked={true} />
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Sound alarm if the flow volume can not be reached within the maximum time">Low-flow alarm</label>
                    <Toggle isChecked={true} />
                  </div>
                  <div className="device-option-container">
                    <label>(Max) pumping duration (sec)</label>
                    <div className="device-number-group">
                      <input type="number" className="device-number-input" defaultValue="15"></input>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label>Pumping frequency (mins)</label>
                    <div className="device-number-group">
                      <input type="number" className="device-number-input" defaultValue="15"></input>
                    </div>
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Invert relay logic">Invert pump relay logic</label>
                    <Toggle isChecked={true} />
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
                    <label data-tip="Sound alarm when triggered">Alarm when triggered</label>
                    <Toggle isChecked={true} />
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Send email when triggered">Email when triggered</label>
                    <Toggle isChecked={true} />
                  </div>
                  <div className="device-option-container">
                    <label data-tip="Invert sensor logic">Invert sensor logic</label>
                    <Toggle isChecked={true} />
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
                    <Toggle isChecked={true} />
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