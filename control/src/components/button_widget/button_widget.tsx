import { ReactNode } from "react";
import button_widget_styles from "./button_widget.module.css"; 

type button_widget_props = {
    className?: string;
    children: ReactNode;
    onClick: (params:any) => void;
}

export default function ButtonWidget(props: button_widget_props) {

    return (
        <button onClick={props.onClick} className={`${button_widget_styles.button_widget} ${props.className} `}>
            { props.children }
        </button>
    )

}