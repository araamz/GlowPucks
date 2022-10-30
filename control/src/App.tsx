import { useState } from 'react'
import reactLogo from './assets/react.svg'
import './App.css'
import { NavLink, Outlet, useParams } from 'react-router-dom'

function App() {

  let { device_id, group_id } = useParams()

  return (
    <div className="App">
      <nav>
        <NavLink end to={"/"} className={({isActive}) => isActive ? "control_panel_navlink_active" : "control_panel_navlink_unactive"}>
          Control Panel
        </NavLink>
        { device_id ? <div className="device_configuration_label"> { device_id ? <p>Device {device_id} Configuration</p> : null } </div> : null}
        { group_id ? <div className="group_configuration_label"> { group_id ? <p>Group {group_id} Configuration</p> : null } </div> : null}
      </nav>
      <div>
        <Outlet />
      </div>
    </div>
  )
}

export default App
