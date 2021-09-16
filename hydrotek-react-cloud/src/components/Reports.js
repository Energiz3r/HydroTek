import { connect } from 'react-redux'
import { serverLocation } from '../config'
import { Link } from 'react-router-dom'
import CanvasJSReact from '../utils/canvasjs-2.3.1/canvasjs.react'
var CanvasJS = CanvasJSReact.CanvasJS;
var CanvasJSChart = CanvasJSReact.CanvasJSChart;

class Reports extends React.Component {
  constructor(props) {
    super(props)
    this.state = {

    }
  }
  render() {
    const {initialRoute,initialRouteRedirect} = this.props.UI
    return (
      <div>
        
        <p>Device Reports</p>
        <p>Initial Route: {initialRoute}</p>
        <p>Initial Set: {initialRouteRedirect ? 'yes' : 'no'}</p>
        <Link to={serverLocation + "devices"}>
          <p>Devices</p>
        </Link>
      </div>
    )
  }
}

export default connect(state=>state)(Reports)