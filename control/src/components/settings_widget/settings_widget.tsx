import ButtonWidget from "../button_widget/button_widget";
import settings_widegt_styles from "./settings_widget.module.css"; 
 
type setting_widet_props = {
    onClick: (params:any) => void;
    className?: string;
}

export default function SettingsWidget(props: setting_widet_props) {

    return (
        <ButtonWidget onClick={props.onClick} className={`${props.className} ${settings_widegt_styles.settings_widget}`}>
            <span className={`material-symbols-outlined ${settings_widegt_styles.settings_icon}`}>
                settings
            </span>
        </ButtonWidget>
    )

}