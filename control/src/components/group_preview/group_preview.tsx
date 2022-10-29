import Graphic from "../graphic/graphic";
import SettingsWidget from "../settings_widget/settings_widget";
import group_preview_styles from "./group_preview.module.css";

export default function GroupPreview() {

    const click_handler = () => {
        console.log("Click from device " + "group")
        
    }

    return (
        <div className={group_preview_styles.group_preview}>
            <SettingsWidget onClick={click_handler}/>
            <p className={group_preview_styles.label}>
                Group 1
            </p>
            <div className={group_preview_styles.devices_container}>
                <Graphic className={group_preview_styles.graphic} active={true} color="red"/>
                <Graphic className={group_preview_styles.graphic} active={true} color="red"/>
                <Graphic className={group_preview_styles.graphic} active={true} color="red"/>
                <Graphic className={group_preview_styles.graphic} active={true} color="red"/>
                <Graphic className={group_preview_styles.graphic} active={true} color="red"/>
            </div>
        </div>
    )

}