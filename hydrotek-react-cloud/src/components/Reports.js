import { connect } from 'react-redux'
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
    return (
      <div><p>hello</p></div>
    )
  }
}

export default connect(state=>state)(Reports)