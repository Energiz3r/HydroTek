import React from 'react'
import { connect } from 'react-redux'
import {setLocation, setLastLocation} from '../actions/uiActions'
import animatedComponent from '../utils/animatedComponent'

class Home extends React.Component {
  constructor(props) {
    super(props)
    this.state = {
      headerCollapsed: false,
      collapseOverride: false,
      loggedIn: window.loggedIn
    }
  }
  navClick = (location) => {
    this.props.dispatch(setLocation(location))
  }
  componentDidUpdate(prevProps) {
    if (prevProps.ui.location != this.props.ui.location) {
      const loc = this.props.ui.location
      setTimeout(()=> {
        this.setState({
          headerCollapsed: false //loc != 'home' && loc != 'login' && loc != 'information'
        })
      },1)
    }    
  }
  render() {
    return (

      <div className='header-parent'>

        {this.state.headerCollapsed ?

          <div className='header-container header-container-collapsed'>
            <div className="collapse-button-container"><button className='button-base button-collapse' onClick={() => {
              this.setState({
                headerCollapsed: !this.state.headerCollapsed
              })
            }} ><i className="fas fa-angle-double-down"></i></button></div>
          </div>
      
        :       
          
          <div className='header-container header-container-expanded'>
            <h1 className='header-title-text'>{this.props.ui.appName}</h1>
            <p className='header-link-text'>
              <a href='#' onClick={() => {this.navClick('home')}} className={'header-link' + (this.props.ui.location == 'home' ? ' header-active-link' : '')}>Home</a>
              {!this.state.loggedIn ?
                <a href='#' onClick={() => {this.navClick('login')}} className={'header-link' + (this.props.ui.location == 'login' ? ' header-active-link' : '')}>Login</a> :
                <a href='#' onClick={() => {this.navClick('logout')}} className='header-link'>Logout</a> }
                <a href='#' onClick={() => {this.navClick('information')}} className={'header-link' + (this.props.ui.location == 'information' ? ' header-active-link' : '')}>Information</a>
            </p>
            {/* <div className="button-container-header"><button className='button-base' onClick={this.goToCalculations} >Go To Calculations</button></div> */}
          </div>
        }
      </div>
    )
  }
}

export default animatedComponent(connect(state=>state)(Home))