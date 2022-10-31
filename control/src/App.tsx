import { useState } from 'react'
import reactLogo from './assets/react.svg'
import app_styles from './App.module.css'
import { NavLink, Outlet, useParams } from 'react-router-dom'

function App() {

  let { device_id, group_id } = useParams()

  return (
    <div className={app_styles.app}>
      <nav>
        <NavLink end to={"/"} className={({isActive}) => isActive ? `${app_styles.control_panel_navlink} ${app_styles.control_panel_navlink_active}` : `${app_styles.control_panel_navlink} ${app_styles.control_panel_navlink_unactive}`}>
          Control Panel
        </NavLink>
        { device_id ? <div className={app_styles.page_label}> { device_id ? <p>Device {device_id} Configuration</p> : null } </div> : null}
        { group_id ? <div className={app_styles.page_label}> { group_id ? <p>Group {group_id} Configuration</p> : null } </div> : null}
      </nav>
      <div>
        <Outlet />
      </div>
    </div>
  )
}

export default App
