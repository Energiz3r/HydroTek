import React from 'react'
import animatedComponent from '../utils/animatedComponent'
import {setLocation} from '../actions/uiActions'
import {connect} from 'react-redux'

import CanvasJSReact from '../utils/canvasjs-2.3.1/canvasjs.react'
var CanvasJS = CanvasJSReact.CanvasJS;
var CanvasJSChart = CanvasJSReact.CanvasJSChart;

class Charts extends React.Component {
  constructor(props) {
    super(props)
    this.state = {
      filter: 'last4Hours',
      serverFilter: window.timePeriod,
      devices: window.devices,
      deviceSelected: window.devices[0].deviceId
    }
  }

  getFloatState = () => {
    var output = {
      float1: this.getFilteredData().float1[this.getFilteredData().float1.length - 1].y,
      float2: this.getFilteredData().float2[this.getFilteredData().float2.length - 1].y
    }
    return output    
  }
  
  getFilteredData = () => {

    var data = window.dataPoints.filter(datapoint=>datapoint.deviceId == this.state.deviceSelected);
    const now = Date.now()

    //get a millisecond value for the required period of time, which we can use to compare against the timestamp for filtering
    const hours = 60 * 60 * 1000
    let filterBy = 365 * 24 * hours //max filter range is last 12 months
    if (this.state.filter == 'last24') {
      filterBy = 24 * hours
    } else if (this.state.filter == 'last7days') {
      filterBy = 7 * 24 * hours
    } else if (this.state.filter == 'last30days') {
      filterBy = 30 * 24 * hours
    } else if (this.state.filter == 'last4Hours') {
      filterBy = 4 * hours
    } else if (this.state.filter == 'lastHour') {
      filterBy = 1 * hours
    }
    
    //apply the date range filter
    if (this.state.filter != 'all') {
      data = data.filter(datapoint=>datapoint.datestamp > now - filterBy)
    }

    let output = {
      temp1:[],
      temp2:[],
      humidity1:[],
      humidity2:[],
      float1:[],
      float2:[],
      flow1:[],
      flow2:[]
    }

    data.map((datapoint)=>{
      output.temp1.push({'y':datapoint.temp1, 'x':datapoint.datestamp})
      output.temp2.push({'y':datapoint.temp2, 'x':datapoint.datestamp})
      output.humidity1.push({'y':datapoint.humidity1, 'x':datapoint.datestamp})
      output.humidity2.push({'y':datapoint.humidity2, 'x':datapoint.datestamp})
      output.float1.push({'y':datapoint.float1, 'x':datapoint.datestamp})
      output.float2.push({'y':datapoint.float2, 'x':datapoint.datestamp})
      output.flow1.push({'y':datapoint.flow1 / 10, 'x':datapoint.datestamp})
      output.flow2.push({'y':datapoint.flow2 / 10, 'x':datapoint.datestamp})
    })
    
    //console.log(output)

    return output
    
  }
  render() {
    const chartType = 'spline'
    const bothCharts = {
      theme: "light2",
      animationEnabled: true,
      zoomEnabled: true,
      exportEnabled: true,
      axisY: {
        title: "Values"
      },
      axisX: {
        valueFormatString: "hh:mm:ss tt"
      },
      toolTip: {
        shared: true
      },
      legend: {
        verticalAlign: "center",
        horizontalAlign: "right",
        reversed: true,
        cursor: "pointer"
      }
    }
    const bothData = {
      type: chartType,
      showInLegend: true,
      xValueType: "dateTime",
      xValueFormatString: "YYYY/MM/DD hh:mm:ss tt"
    }
    const plant1 = {
      ...bothCharts,
      title: {
        text: "Plant 1"
      },
      data: [
        {
          ...bothData,
          name: "Humidity",
          dataPoints: this.getFilteredData().humidity1,
          toolTipContent: "{x}</br><span style='\"'color: {color};'\"'>{name}</span>, <strong>{y}</strong>"
        },
        {
          ...bothData,
          name: "Temp",
          dataPoints: this.getFilteredData().temp1,
          toolTipContent: "<span style='\"'color: {color};'\"'>{name}</span>, <strong>{y}</strong>"
        },
        {
          ...bothData,
          name: "Flow",
          dataPoints: this.getFilteredData().flow1,
          toolTipContent: "<span style='\"'color: {color};'\"'>{name}</span>, <strong>{y}</strong>"
        }
      ]
    }
    const plant2 = {
      ...bothCharts,
      title: {
        text: "Plant 2"
      },
      data: [
        {
          ...bothData,
          name: "Humidity",
          dataPoints: this.getFilteredData().humidity2,
          toolTipContent: "{x}</br><span style='\"'color: {color};'\"'>{name}</span>, <strong>{y}</strong>"
        },
        {
          ...bothData,
          name: "Temp",
          dataPoints: this.getFilteredData().temp2,
          toolTipContent: "<span style='\"'color: {color};'\"'>{name}</span>, <strong>{y}</strong>"
        },
        {
          ...bothData,
          name: "Flow",
          dataPoints: this.getFilteredData().flow2,
          toolTipContent: "<span style='\"'color: {color};'\"'>{name}</span>, <strong>{y}</strong>"
        }
      ]
    }
    return (

      window.dataPoints.length > 0 ? 
      <div className='main-menu-container'>
        <h2>Charts</h2>
        <table className='charts-option-table'>
          <tbody>
            <tr><th>Retrieve from server:</th><th>Data to display:</th></tr>
            <tr>
              <td>
                <div className="button-container">
                  <form action='index.php' method='post'>
                    <p><input type='submit' name="fetchPeriod" className={'button-base button-base-input ' + (this.state.serverFilter == 'lastYear' ? 'button-selected' : 'button-deselected')} value="Last 12 Months" />
                    <input type='submit' name="fetchPeriod" className={'button-base button-base-input ' + (this.state.serverFilter == 'lastQuarter' ? 'button-selected' : 'button-deselected')} value="Last 3 Months" />
                    <input type='submit' name="fetchPeriod" className={'button-base button-base-input ' + (this.state.serverFilter == 'lastMonth' ? 'button-selected' : 'button-deselected')} value="Last Month" /> 
                    <input type='submit' name="fetchPeriod" className={'button-base button-base-input ' + (this.state.serverFilter == 'lastWeek' ? 'button-selected' : 'button-deselected')} value="Last 7 Days" /> </p>
                  </form>
                </div>
              </td>
              <td>
                <div className="button-container">
                  <p>
                    <button className={'button-base ' + (this.state.filter == 'all' ? 'button-selected' : 'button-deselected')} onClick={() => { this.setState({...this.state, filter: 'all'}) }} >All Time</button>
                    <button className={'button-base ' + (this.state.filter == 'last30days' ? 'button-selected' : 'button-deselected')} onClick={() => { this.setState({...this.state, filter: 'last30days'}) }} >Last 30 days</button>
                    <button className={'button-base ' + (this.state.filter == 'last7days' ? 'button-selected' : 'button-deselected')} onClick={() => { this.setState({...this.state, filter: 'last7days'}) }} >Last 7 days</button>
                    <button className={'button-base ' + (this.state.filter == 'last24' ? 'button-selected' : 'button-deselected')} onClick={() => { this.setState({...this.state, filter: 'last24'}) }} >Last 24hrs</button>
                    <button className={'button-base ' + (this.state.filter == 'last4Hours' ? 'button-selected' : 'button-deselected')} onClick={() => { this.setState({...this.state, filter: 'last4Hours'}) }} >Last 4hrs</button>
                    <button className={'button-base ' + (this.state.filter == 'lastHour' ? 'button-selected' : 'button-deselected')} onClick={() => { this.setState({...this.state, filter: 'lastHour'}) }} >Last Hour</button>
                  </p>
                </div>
              </td>
            </tr>
          </tbody>
        </table>

        <div>
          <table className='run-off-table'>
            <tbody>
              <tr>
                <th colSpan="2">Run-off tanks (most recent):</th>
              </tr>
              <tr>
                {this.getFilteredData().humidity1.length > 0 && 
                  <td>Plant 1: <i className={this.getFloatState().float1 > 0 ? "float-full" : "float-normal"}>{this.getFloatState().float1 > 0 ? "FULL" : "OK"}</i> </td>
                  }
                {this.getFilteredData().humidity2.length > 0 && 
                  <td>Plant 2: <i className={this.getFloatState().float2 > 0 ? "float-full" : "float-normal"}>{this.getFloatState().float2 > 0 ? "FULL" : "OK"}</i></td>
                  }
              </tr> 
            </tbody>  
          </table>
        </div>

        <div className="charts-container">
          {this.getFilteredData().humidity1.length > 0 ? 
            <CanvasJSChart options = {plant1} /> :
            <h2>No data for selected time period.</h2>
            }
          {this.getFilteredData().humidity2.length > 0 &&
            <CanvasJSChart options = {plant2} /> 
            }
		    </div>
      </div> :
      <div className='main-menu-contrainer'>
        <h2 className="float-full">No data on server for selected period.</h2>
      </div>
    )
  }
}

export default animatedComponent(connect(state=>state)(Charts))