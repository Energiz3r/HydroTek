import { connect } from 'react-redux'
import { setRoute } from '../actions/UIActions'
import { serverAPILocation } from '../config'

class Listing extends React.Component {
  constructor(props) {
    super(props)
    this.state = {
      deviceList: [],
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
  onAddClick = () => {
    this.setState({
      ...this.state,
      waitingForAdd: true
    })
    fetch(serverAPILocation, {
      method: 'POST',
      cache: 'no-cache',
      headers: {
        'Content-Type': 'application/json'
      },
      body: JSON.stringify({
        "action": "createNewDevice"
      })
    })
      .then(result => result.json())
      .then(
        (result) => {
          if (result.deviceCreated === "1") {
            console.log(result)

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
  render() {
    const { deviceList } = this.state
    return (
      <div>
          <h4>Devices</h4>
          <p>Hello. Route is: {this.props.location.pathname}</p>
          <button className='button-default' onClick={this.onHomeClick}>Home</button>
          { deviceList.length() > 1 && <p>No devices</p> }

          <button className='button-default' onClick={this.onAddClick}>Add</button>

      </div>
    )
  }
}

export default connect(state=>state)(Listing)