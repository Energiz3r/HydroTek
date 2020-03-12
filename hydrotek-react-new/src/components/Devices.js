import { connect } from 'react-redux'
import { setRoute } from '../actions/UIActions'
import { serverAPILocation } from '../config'
import Toggle from '../utils/Toggle'
import ReactTooltip from "react-tooltip"
import { demoDevices } from "../utils/demo-device"

class Listing extends React.Component {
  constructor(props) {
    super(props)
    this.state = {
      deviceList: demoDevices,
      waitingForAdd: false,
      loadingDevices: true
    }
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
  onTempCaret = index => e => {
    const { deviceList } = this.state
    let x = deviceList[index.device].devicePlants[index.plant]
    x.tempShowing = !x.tempShowing
    if (x.tempShowing) {
      /* x.tempShowing = false; */ x.lampShowing = false; x.pumpShowing = false; x.floatShowing = false; x.flowShowing = false;
    }
    this.setState({ ...this.state, deviceList })
  }
  onLampCaret = index => e => {
    const { deviceList } = this.state
    let x = deviceList[index.device].devicePlants[index.plant]
    x.lampShowing = !x.lampShowing
    if (x.lampShowing) {
      x.tempShowing = false; /* x.lampShowing = false; */ x.pumpShowing = false; x.floatShowing = false; x.flowShowing = false;
    }
    this.setState({ ...this.state, deviceList })
  }
  onPumpCaret = index => e => {
    const { deviceList } = this.state
    let x = deviceList[index.device].devicePlants[index.plant]
    x.pumpShowing = !x.pumpShowing
    if (x.pumpShowing) {
      x.tempShowing = false; x.lampShowing = false; /* x.pumpShowing = false; */ x.floatShowing = false; x.flowShowing = false;
    }
    this.setState({ ...this.state, deviceList })
  }
  onFloatCaret = index => e => {
    const { deviceList } = this.state
    let x = deviceList[index.device].devicePlants[index.plant]
    x.floatShowing = !x.floatShowing
    if (x.floatShowing) {
      x.tempShowing = false; x.lampShowing = false; x.pumpShowing = false; /* x.floatShowing = false; */ x.flowShowing = false;
    }
    this.setState({ ...this.state, deviceList })
  }
  onFlowCaret = index => e => {
    const { deviceList } = this.state
    let x = deviceList[index.device].devicePlants[index.plant]
    x.flowShowing = !x.flowShowing
    if (x.flowShowing) {
      x.tempShowing = false; x.lampShowing = false; x.pumpShowing = false; x.floatShowing = false; /* x.flowShowing = false; */
    }
    this.setState({ ...this.state, deviceList })
  }
  render() {
    const { deviceList } = this.state
    return (
      <div className="device-content">
        <h2>Device List</h2>

        {demoDevices.map((device, i)=> {
          return(
          <div className="device-container" key={device.deviceId}>

            <div className="device-option-container device-label-container">
              <i className={"fas fa-microchip device-icon" + (device.deviceShowing ? " icon-selected" : "")}
              onClick={this.onDeviceCaret({device: i})}></i>
              <input className="device-label-input" type="text" placeholder="Device Label" data-tip="A friendly name to identify the device, eg. 'Front Room'"
                value={device.deviceLabel} onChange={()=>{this.setState()}}></input>
              <i className={"fas fa-caret-right device-caret" + (device.deviceShowing ? " fa-rotate-90" : "")}
              onClick={this.onDeviceCaret({device: i})}></i>
            </div>

            {device.deviceShowing && <div>
            <div className="device-option-container">
              <label data-tip="Controls whether the device uploads sensor readings at all">Enable Online Logging</label>
              <Toggle isChecked={device.loggingEnabled} />
            </div>
            <div className="device-option-container">
              <label data-tip="Frequency, in minutes, of how often to upload status to server">Upload frequency (mins)</label>
              <div className="device-number-group">
                <input type="number" className="device-number-input" defaultValue="1"></input>
                {/* <button className='button-default button-device-number'><i className="fas fa-plus"></i></button>
                <button className='button-default button-device-number'><i className="fas fa-minus"></i></button> */}
              </div>
            </div>
            <div className="device-option-container device-option-split-row">
              <label data-tip="Address to send device alerts to (if enabled)">Alert email address</label>
              <input className="device-email-input" type="text" placeholder="Email address"></input>
            </div>

            {device.devicePlants.map((plant, j)=>{

              return (
              <div className='device-sub-container' key={plant.plantId}>
                <div className="device-option-container device-label-container">
                  <i className={"fas fa-leaf device-icon" + (plant.plantShowing ? " icon-selected" : "")}
                  onClick={this.onPlantCaret({device: i, plant: j})}></i>
                  <input className="device-label-input plant-name-input" type="text" defaultValue={plant.plantName}></input>
                  <i className={"fas fa-caret-right device-caret" + (plant.plantShowing ? " fa-rotate-90" : "")}
                  onClick={this.onPlantCaret({device: i, plant: j})}></i>
                </div>

                {plant.plantShowing && <div>

                {/* TEMP */}

                <div className="device-option-container device-label-container">
                  <i className={"fas fa-thermometer-half device-icon" + (plant.tempShowing ? " icon-selected" : "")}
                  onClick={this.onTempCaret({device: i, plant: j})}></i>
                  <p className='device-sub-title'>Temperature / Humidity</p>
                  <i className={"fas fa-caret-right device-caret" + (plant.tempShowing ? " fa-rotate-90" : "")}
                  onClick={this.onTempCaret({device: i, plant: j})}></i>
                </div>
                {plant.tempShowing && <div>
                  <div className="device-option-container">
                    <label data-tip="Enable / Disable the temp / humidity sensor">Temp / humidity sensor enable</label>
                    <Toggle isChecked={plant.tempEnabled} />
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

                <div className="device-option-container device-label-container">
                  <i className={"far fa-lightbulb device-icon" + (plant.lampShowing ? " icon-selected" : "")}
                  onClick={this.onLampCaret({device: i, plant: j})}></i>
                  <p className='device-sub-title'>Grow Lamp</p>
                  <i className={"fas fa-caret-right device-caret" + (plant.lampShowing ? " fa-rotate-90" : "")}
                  onClick={this.onLampCaret({device: i, plant: j})}></i>
                </div>
                {plant.lampShowing && <div>
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

                <div className="device-option-container device-label-container">
                  <i className={"fas fa-tint device-icon" + (plant.pumpShowing ? " icon-selected" : "")}
                  onClick={this.onPumpCaret({device: i, plant: j})}></i>
                  <p className='device-sub-title'>Nutrient Pump</p>
                  <i className={"fas fa-caret-right device-caret" + (plant.pumpShowing ? " fa-rotate-90" : "")}
                  onClick={this.onPumpCaret({device: i, plant: j})}></i>
                </div>
                {plant.pumpShowing && <div>
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

                <div className="device-option-container device-label-container">
                  <i className={"fas fa-fill-drip device-icon" + (plant.floatShowing ? " icon-selected" : "")}
                  onClick={this.onFloatCaret({device: i, plant: j})}></i>
                  <p className='device-sub-title'>Float Sensor</p>
                  <i className={"fas fa-caret-right device-caret" + (plant.floatShowing ? " fa-rotate-90" : "")}
                  onClick={this.onFloatCaret({device: i, plant: j})}></i>
                </div>
                {plant.floatShowing && <div>
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

                <div className="device-option-container device-label-container">
                  <i className={"fas fa-water device-icon" + (plant.flowShowing ? " icon-selected" : "")}
                  onClick={this.onFlowCaret({device: i, plant: j})}></i>
                  <p className='device-sub-title'>Flow Sensor</p>
                  <i className={"fas fa-caret-right device-caret" + (plant.flowShowing ? " fa-rotate-90" : "")}
                  onClick={this.onFlowCaret({device: i, plant: j})}></i>
                </div>
                {plant.flowShowing && <div>
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

export default connect(state=>state)(Listing)