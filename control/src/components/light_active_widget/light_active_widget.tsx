import Widget from "../widget/widget";
import light_active_widget_styles from "./light_active_widget.module.css"; 
 
type light_active_widget_props = {
    active: boolean
}

export default function LightActiveWidget(props: light_active_widget_props) {

    const show_light_status_text = () => {

        let enabled_text = "ACTIVE"
        let disabled_text = "INACTIVE"
        
        return props.active ? enabled_text : disabled_text
    }

    const show_light_active_icon = () => {

        let enabled_class = light_active_widget_styles.active
        let disabled_class = light_active_widget_styles.inactive
        
        return props.active ? enabled_class : disabled_class

    }

    return (
        <Widget className={light_active_widget_styles.light_active_widget}>
            <p className={light_active_widget_styles.status_text}>
                { show_light_status_text() }
            </p>
            <div className={light_active_widget_styles.active_icon}>
                <span className={`material-symbols-outlined ${show_light_active_icon()}`}>
                    light_mode
                </span>
            </div>
        </Widget>
    )

}