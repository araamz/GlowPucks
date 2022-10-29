import { ReactNode } from "react";
import widget_styles from "./widget.module.css"; 

type widget_props = {
    className?: string;
    children: ReactNode;
}

export default function Widget(props: widget_props) {

    return (
        <div className={`${widget_styles.widget} ${props.className}`}>
            {props.children}
        </div>
    )

}