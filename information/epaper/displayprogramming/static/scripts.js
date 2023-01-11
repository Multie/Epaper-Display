/*
 *   Copyright (c) 2023 Malte Hering
 *   All rights reserved.

 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at

 *   http://www.apache.org/licenses/LICENSE-2.0

 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */
var baseurl = "";

function getData(path) {
    return new Promise((resolve, reject) => {
        const req = new XMLHttpRequest();
        req.addEventListener("load", () => {
            resolve(req.responseText);
        });
        req.open("GET", baseurl + path);
        req.send();
    });
}

function getData(path, consume) {
    return new Promise((resolve, reject) => {
        fetch(baseurl + path)
            .then(res => {
                consume(res.body.getReader())
            }).then(() => {
                console.log("consumed the entire body without keeping the whole thing in memory!");
                resolve()
            }).catch(e => {
                console.log("something went wrong: " + e);
                reject(e);
            });
    });
}

function postData(path, text) {
    return new Promise((resolve, reject) => {
        
        let array = new Uint8Array(text.length);
        for (let a=0; a< text.length;a++) {
            array[a] = text.charCodeAt(a);
        }
       
        const req = new XMLHttpRequest();
        req.addEventListener("load", () => {
            resolve(req.responseText);
        });
        req.open("POST", baseurl + path,false);
        req.send(array);
/*
        fetch(baseurl + path, {
            method: 'POST', // *GET, POST, PUT, DELETE, etc.
            mode: 'cors', // no-cors, *cors, same-origin
            cache: 'no-cache', // *default, no-cache, reload, force-cache, only-if-cached
            credentials: 'same-origin', // include, *same-origin, omit
            headers: {
              'Content-Type': 'arraybuffer'
              // 'Content-Type': 'application/x-www-form-urlencoded',
            },
            redirect: 'follow', // manual, *follow, error
            referrerPolicy: 'no-referrer', // no-referrer, *no-referrer-when-downgrade, origin, origin-when-cross-origin, same-origin, strict-origin, strict-origin-when-cross-origin, unsafe-url
            body: text // body data type must match "Content-Type" header
          }).then((response)=>{
            resolve(response.text());
          });*/
    });
}

const Parameter = {
    hold: "z",
    type: "t",
    x: "x",
    y: "y",
    color: "c",
    x1: "x1",
    y1: "y1",
    x2: "x2",
    y2: "y2",
    width: "w",
    height: "h",
    radius: "r",
    size: "s",
    label: "l",
    name: "n",
    fill: "f",
    align: "a"
}
class DisplayOption {
    constructor() {
        this.name = "";
        this.options = {
            [Parameter.hold]: {
                type: "checkbox",
                name: "Hold",
                value: "",
                true: "1",
                false: ""
            }
        };
        this.id = Date.now() + "#" + Math.round(Math.random() * 10000).toString().padStart(5, "0");
        this.type = "---";
        this.hide = false;
    }

    getOutputValue(name, value) {
        console.log(name + String.fromCharCode(187) + value + String.fromCharCode(166))

        if (value == null || value == undefined) {
            return "";
        }
        if (value.toString().length == 0) {
            return "";
        }
        if (typeof (value) == "number" && isNaN(value)) {
            console.log("is nan")
            return "";
        }

        return name + String.fromCharCode(187) + value.toString() + String.fromCharCode(166);

    }
    getOutput() {
        let output = "";
        if (this.options.z) {
            let key = "z";
            output += this.getOutputValue(key, this.options[key].value);
        }
        output += this.getOutputValue("t", this.type);

        let keys = Object.keys(this.options);
        console.log(this.options);
        let indexofz = keys.indexOf("z");
        if (indexofz >= 0) {
            keys.splice(indexofz, 1);
        }
        keys.forEach(key => {
            output += this.getOutputValue(key, this.options[key].value);
        });

        return output;
    }
    createOptionsHtml(parrent) {
        let itemroot = document.createElement("div");
        itemroot.classList.add("itemGroupVertical")
        itemroot.id = this.id;
        parrent.appendChild(itemroot);

        let item = document.createElement("div");
        item.classList.add("item");
        itemroot.appendChild(item);

        let label = document.createElement("div");
        label.classList.add("itemLabel");
        label.innerHTML = this.name;
        item.appendChild(label);

        let value = document.createElement("div");
        value.classList.add("itemValue");
        item.appendChild(value);

        let button = document.createElement("button");
        button.innerHTML = "Add";
        button.setAttribute("type", this.type);
        button.onclick = ((ev) => {
            let button = ev.target;
            let type = button.getAttribute("type");
            display.addContent(type);
        });
        value.appendChild(button);

    }
    createContentHtml(parrent) {

        let headernames = [];
        headernames.push("controls");
        headernames = headernames.concat(Object.keys(Parameter));
        console.log(headernames);

        let row = document.createElement("tr");
        row.id = this.id;


        parrent.appendChild(row);
        headernames.forEach(key => {
            let colum = document.createElement("td");

            row.appendChild(colum);

            let cell = document.createElement("div");
            cell.classList.add("contentTableCell")
            colum.appendChild(cell);

            if (key == "controls") {
                let removebutton = document.createElement("button");
                removebutton.innerHTML = "Remove"
                removebutton.setAttribute("name", this.id);
                removebutton.onclick = ((ev) => {
                    // get rid of html
                    let id = ev.target.getAttribute("name");
                    let itemroot = document.getElementById(id);
                    itemroot.remove();
                    // get rid of array element
                    let index = display.content.findIndex((element) => {
                        return element.id == id;
                    });
                    if (index >= 0) {
                        display.content.splice(index, 1);
                        display.createOutput();
                    }

                })
                cell.appendChild(removebutton);

                let MoveBackButton = document.createElement("button");
                MoveBackButton.innerHTML = "/\\"
                MoveBackButton.setAttribute("name", this.id);
                MoveBackButton.onclick = ((ev) => {
                    // get rid of html
                    let id = ev.target.getAttribute("name");
                    let index = display.content.findIndex((element) => {
                        return element.id == id;
                    });
                    if (index >= 0) {
                        let element = display.content.splice(index, 1);
                        display.createOutput();
                        display.content.splice(index - 1, -1, element[0]);
                    }
                    display.createContentHtml();
                    display.createOutput();
                })
                cell.appendChild(MoveBackButton);

                let MoveForwardButton = document.createElement("button");
                MoveForwardButton.innerHTML = "\\/"
                MoveForwardButton.setAttribute("name", this.id);
                MoveForwardButton.onclick = ((ev) => {
                    // get rid of html
                    let id = ev.target.getAttribute("name");
                    let index = display.content.findIndex((element) => {
                        return element.id == id;
                    });
                    if (index >= 0) {
                        let element = display.content.splice(index, 1);
                        display.createOutput();
                        display.content.splice(index + 1, -1, element[0]);
                    }
                    display.createContentHtml();
                    display.createOutput();
                })
                cell.appendChild(MoveForwardButton);

                let hidebutton = document.createElement("button");
                hidebutton.innerHTML = "Hide"
                hidebutton.setAttribute("name", this.id);
                hidebutton.onclick = ((ev) => {
                    // get rid of html
                    let id = ev.target.getAttribute("name");
                    let itemroot = document.getElementById(id);
                    itemroot.remove();
                    let element = display.content.find((element) => {
                        return element.id == id;
                    });
                    element.hide = true;
                })
                cell.appendChild(hidebutton);
            }
            if (key == "type") {
                cell.innerHTML = this.name;
            }
            else {
                let option = this.options[Parameter[key]];

                if (option) {

                    let input = null;
                    if (option.type == "number") {
                        input = document.createElement("input");
                        input.type = "number";
                        input.value = option.value;
                        input.readOnly = (option.readOnly == true);
                        console.log(option.readOnly);
                        input.onchange = ((ev) => {
                            let key = ev.target.getAttribute("name");
                            let option = this.options[key];
                            let sign = "";
                            if (ev.target.value.startsWith("+")) {
                                sign = "+"
                            }
                            else if (ev.target.value.startsWith("-")) {
                                sign = "-"
                            }
                            if (sign.length == 0) {
                                if (option.min != null || option.min != undefined) {
                                    if (ev.target.value < option.min) {
                                        ev.target.value = option.min;
                                    }
                                }
                                if (option.max != null || option.max != undefined) {
                                    if (ev.target.value > option.max) {
                                        ev.target.value = option.max;
                                    }
                                }
                            }
                            let floatValue = parseFloat(ev.target.value);
                            if (floatValue < 0) {
                                floatValue = floatValue * -1;
                            }
                            if (!isNaN(floatValue)) {
                                option.value = sign + floatValue.toString();
                            }
                            else {
                                option.value = "";
                            }
                            display.createOutput();

                        });
                        if (option.min != null || option.min != undefined) {
                            input.min = option.max * -1;
                        }
                        if (option.max != null || option.max != undefined) {
                            input.max = option.max;
                        }
                    }
                    else if (option.type == "text") {
                        input = document.createElement("input");
                        input.type = "text";
                        input.value = option.value;
                        input.onchange = ((ev) => {
                            let key = ev.target.getAttribute("name");
                            this.options[key].value = ev.target.value;
                            display.createOutput();

                        });
                    }
                    else if (option.type == "select") {
                        input = document.createElement("select");
                        let opt = document.createElement("option");
                        opt.value = "";
                        opt.innerHTML = "-";
                        input.appendChild(opt);

                        option.options.forEach(element => {
                            let opt = document.createElement("option");
                            opt.value = element.value;
                            opt.innerHTML = element.label;
                            input.appendChild(opt);
                        });

                        input.onchange = ((ev) => {
                            let key = ev.target.getAttribute("name");
                            this.options[key].value = ev.target.value;
                            display.createOutput();

                        });
                        input.value = option.value;
                    }
                    else if (option.type == "checkbox") {
                        input = document.createElement("input");
                        input.type = "checkbox";
                        input.checked = option.value == option.true;
                        input.onchange = (ev => {
                            let key = ev.target.getAttribute("name");
                            this.options[key].value = ev.target.checked ? this.options[key].true : this.options[key].false;
                            console.log("change", this.options[key].value);
                            display.createOutput();
                        });
                    }
                    else if (option.type == "poly") {

                        let button = document.createElement("button");
                        cell.appendChild(button);
                        button.setAttribute("name", Parameter[key]);
                        button.innerHTML = "Add"
                        cell.classList.add("poly");
                        this.polyCreateOutput = ((option) => {
                            let text = "";
                            let count = 0;
                            option.values.forEach((value) => {
                                if (value.x != "" && value.y != "") {
                                    let num = parseInt(value.x);
                                    if (num < 0) {
                                        num = (num >>> 0)
                                    }
                                    if (num > 32767) {
                                        num = 32767;
                                    }
                                    if (num < -32767) {
                                        num = -32767;
                                    }
                                    let bin = parseInt(num).toString(2).padStart(16, "0");
                                    let len = bin.length;
                                    let upper = bin.substring(len - 16, len - 8);
                                    let lower = bin.substring(len - 8, len);
                                    let upperText = String.fromCharCode(parseInt(upper, 2))
                                    let lowerText = String.fromCharCode(parseInt(lower, 2))
                                    text += lowerText + upperText;
                                    console.log("PolyX",value.x,upperText,lowerText);
                                    num = parseInt(value.y);
                                    if (num < 0) {
                                        num = (num >>> 0)
                                    }
                                    if (num > 32767) {
                                        num = 32767;
                                    }
                                    if (num < -32767) {
                                        num = -32767;
                                    }
                                    bin = parseInt(num).toString(2).padStart(16, "0");
                                    len = bin.length;
                                    upper = bin.substring(len - 16, len - 8);
                                    lower = bin.substring(len - 8, len);
                                    upperText = String.fromCharCode(parseInt(upper, 2))
                                    lowerText = String.fromCharCode(parseInt(lower, 2))
                                    console.log("PolyY",value.y,upperText,lowerText);
                                    text += lowerText + upperText;
                                    count++;
                                }
                            });
                            this.options[Parameter.size].value = count.toString();
                            this.options[Parameter.size].input.value = count.toString();
                            option.value = text;
                        })
                        this.addValue = ((value, index) => {
                            let div = document.createElement("div");
                            cell.appendChild(div);
                            div.classList.add("contentTableInputPoly");
                            let labelx = document.createElement("div");
                            labelx.innerHTML = "X";
                            div.appendChild(labelx);
                            let xinput = document.createElement("input");
                            div.appendChild(xinput);
                            xinput.setAttribute("name", Parameter[key])
                            xinput.setAttribute("index", index);
                            xinput.value = value.x;
                            xinput.type = "number";
                            xinput.onchange = ((ev) => {
                                console.log(ev);
                                let key = ev.target.getAttribute("name");
                                let index = ev.target.getAttribute("index");
                                let option = this.options[key];
                                let value = option.values[index];
                                value.x = ev.target.value;
                                this.polyCreateOutput(option);
                                display.createOutput();
                            });

                            let labely = document.createElement("div");
                            labely.innerHTML = "Y";
                            div.appendChild(labely);
                            let yinput = document.createElement("input");
                            div.appendChild(yinput);
                            yinput.setAttribute("name", Parameter[key])
                            yinput.setAttribute("index", index);
                            yinput.type = "number";
                            yinput.value = value.y;
                            yinput.onchange = ((ev) => {
                                let key = ev.target.getAttribute("name");
                                let index = ev.target.getAttribute("index");
                                let option = this.options[key];
                                let value = option.values[index];
                                value.y = ev.target.value;
                                this.polyCreateOutput(option);
                                display.createOutput();
                            });
                        });



                        button.onclick = (ev) => {
                            let key = ev.target.getAttribute("name");
                            let option = this.options[key];
                            let value = {
                                x: "",
                                y: ""
                            }
                            if (option.values.length < option.max) {
                                option.values.push(value);
                                this.addValue(value, option.values.length - 1);
                            }
                            display.createOutput();
                        };

                        option.values.forEach((value, index) => {
                            this.addValue(value, index);
                        });

                    }
                    if (input) {
                        if (key != option.name.toLocaleLowerCase()) {
                            let label = document.createElement("label");
                            label.innerHTML = option.name;
                            label.for = this.id + "_" + Parameter[key];
                            cell.appendChild(label);
                        }
                        option.input = input;
                        input.id = this.id + "_" + Parameter[key];
                        input.setAttribute("name", Parameter[key]);
                        input.classList.add("contentTableInput");
                        cell.appendChild(input);
                    }
                }
            }

        });
    }
    clone() {
        return new DisplayOption();
    }
}

class DisplayOptionPixel extends DisplayOption {
    constructor() {
        super();
        this.name = "Pixel";
        this.type = "pixel";
        let options = {
            [Parameter.x]: {
                type: "number",
                name: "X",
                min: 0,
                max: 400,
                value: ""
            },
            [Parameter.y]: {
                type: "number",
                name: "Y",
                min: 0,
                max: 300,
                value: ""
            },
            [Parameter.color]: {
                type: "select",
                name: "Color",
                options: [
                    { label: "Black", value: "b" },
                    { label: "Red", value: "r" },
                    { label: "White", value: "w" },
                ],
                value: ""
            }
        };
        this.options = { ...this.options, ...options };

    }
    clone() {
        return new DisplayOptionPixel();
    }
}
class DisplayOptionLine extends DisplayOption {
    constructor() {
        super();
        this.name = "Line";
        this.type = "line";

        let options = {
            [Parameter.x]: {
                type: "number",
                name: "Start",
                min: 0,
                max: 400,
                value: ""
            },
            [Parameter.y]: {
                type: "number",
                name: "Start",
                min: 0,
                max: 300,
                value: ""
            },
            [Parameter.x2]: {
                type: "number",
                name: "End",
                min: 0,
                max: 400,
                value: ""
            },
            [Parameter.y2]: {
                type: "number",
                name: "End",
                min: 0,
                max: 300,
                value: ""
            },
            [Parameter.color]: {
                type: "select",
                name: "Color",
                options: [
                    { label: "Black", value: "b" },
                    { label: "Red", value: "r" },
                    { label: "White", value: "w" },
                ],
                value: ""
            }
        }
        this.options = { ...this.options, ...options };
    }
    clone() {
        return new DisplayOptionLine();
    }


}
class DisplayOptionCircle extends DisplayOption {
    constructor() {
        super();
        this.name = "Circle";
        this.type = "circle";
        let options = {
            [Parameter.x]: {
                type: "number",
                name: "X",
                min: 0,
                max: 400,
                value: ""
            },
            [Parameter.y]: {
                type: "number",
                name: "Y",
                min: 0,
                max: 300,
                value: ""
            },
            [Parameter.radius]: {
                type: "number",
                name: "Radius",
                min: 0,
                value: ""
            },
            [Parameter.color]: {
                type: "select",
                name: "Color",
                options: [
                    { label: "Black", value: "b" },
                    { label: "Red", value: "r" },
                    { label: "White", value: "w" },
                ],
                value: ""
            },
            [Parameter.fill]: {
                type: "select",
                name: "Fill",
                options: [
                    { label: "1", value: "1" },
                    { label: "0", value: "0" },
                ],
                value: "",

            }
        }
        this.options = { ...this.options, ...options };
    }
    clone() {
        return new DisplayOptionCircle();
    }
}
class DisplayOptionCircleSegment extends DisplayOption {
    constructor() {
        super();
        this.name = "Segment";
        this.type = "cicseg";
        let options = {
            [Parameter.x]: {
                type: "number",
                name: "X",
                min: 0,
                max: 400,
                value: ""
            },
            [Parameter.y]: {
                type: "number",
                name: "Y",
                min: 0,
                max: 300,
                value: ""
            },
            [Parameter.radius]: {
                type: "number",
                name: "Start Radius",
                min: 0,
                value: ""
            },
            [Parameter.size]: {
                type: "number",
                name: "End Radius",
                min: 0,
                value: ""
            },
            [Parameter.x2]: {
                type: "number",
                name: "Start Angle",
                min: 0,
                value: ""
            },
            [Parameter.y2]: {
                type: "number",
                name: "End Angle",
                min: 0,
                value: ""
            },
            [Parameter.color]: {
                type: "select",
                name: "Color",
                options: [
                    { label: "Black", value: "b" },
                    { label: "Red", value: "r" },
                    { label: "White", value: "w" },
                ],
                value: ""
            },
            [Parameter.fill]: {
                type: "select",
                name: "Fill",
                options: [
                    { label: "1", value: "1" },
                    { label: "0", value: "0" },
                ],
                value: "",

            }
        }
        this.options = { ...this.options, ...options };
    }
    clone() {
        return new DisplayOptionCircleSegment();
    }
}
class DisplayOptionRectangle extends DisplayOption {
    constructor() {
        super();
        this.name = "Rectangle";
        this.type = "rect";
        let options = {
            [Parameter.x]: {
                type: "number",
                name: "X",
                min: 0,
                max: 400,
                value: ""
            },
            [Parameter.y]: {
                type: "number",
                name: "Y",
                min: 0,
                max: 300,
                value: ""
            },
            [Parameter.width]: {
                type: "number",
                name: "Width",
                min: 0,
                max: 400,
                value: ""
            },
            [Parameter.height]: {
                type: "number",
                name: "Height",
                min: 0,
                max: 300,
                value: ""
            },
            [Parameter.color]: {
                type: "select",
                name: "Color",
                options: [
                    { label: "Black", value: "b" },
                    { label: "Red", value: "r" },
                    { label: "White", value: "w" },
                ],
                value: ""
            },
            [Parameter.fill]: {
                type: "select",
                name: "Fill",
                options: [
                    { label: "1", value: "1" },
                    { label: "0", value: "0" },
                ],
                value: "",

            }
        };
        this.options = { ...this.options, ...options };
    }
    clone() {
        return new DisplayOptionRectangle();
    }
}
class DisplayOptionText extends DisplayOption {
    constructor() {
        super();
        this.name = "Text";
        this.type = "text";
        let options = {
            [Parameter.x]: {
                type: "number",
                name: "X",
                min: 0,
                max: 400,
                value: ""
            },
            [Parameter.y]: {
                type: "number",
                name: "Y",
                min: 0,
                max: 300,
                value: ""
            },
            [Parameter.width]: {
                type: "number",
                name: "Width",
                min: 0,
                max: 400,
                value: ""
            },
            [Parameter.height]: {
                type: "number",
                name: "Height",
                min: 0,
                max: 300,
                value: ""
            },
            [Parameter.color]: {
                type: "select",
                name: "Color",
                options: [
                    { label: "Black", value: "b" },
                    { label: "Red", value: "r" },
                    { label: "White", value: "w" },
                ],
                value: ""
            },
            [Parameter.size]: {
                type: "select",
                name: "Size",
                options: [
                    { label: "8px", value: "b" },
                    { label: "12px", value: "12" },
                    { label: "16px", value: "16" },
                    { label: "20px", value: "20" },
                    { label: "24px", value: "24" },
                ],
                value: ""
            },
            [Parameter.align]: {
                type: "select",
                name: "Align",
                options: [
                    { label: "Top Left", value: "tl" },
                    { label: "Top Center", value: "tc" },
                    { label: "Top Right", value: "tr" },
                    { label: "Center Left", value: "cl" },
                    { label: "Center Center", value: "cc" },
                    { label: "Center Right", value: "cr" },
                    { label: "Bottom Left", value: "bl" },
                    { label: "Bottom Center", value: "bc" },
                    { label: "Bottom Right", value: "br" },
                ],
                value: ""
            },
            [Parameter.label]: {
                type: "text",
                name: "Label",
                value: ""
            }
        };
        this.options = { ...this.options, ...options };
    }
    clone() {
        return new DisplayOptionText();
    }
}
class DisplayOptionValue extends DisplayOption {
    constructor() {
        super();
        this.name = "Value";
        this.type = "value";
        let options = {
            [Parameter.x]: {
                type: "number",
                name: "X",
                min: 0,
                max: 400,
                value: ""
            },
            [Parameter.y]: {
                type: "number",
                name: "Y",
                min: 0,
                max: 300,
                value: ""
            },
            [Parameter.width]: {
                type: "number",
                name: "Width",
                min: 0,
                max: 400,
                value: ""
            },
            [Parameter.height]: {
                type: "number",
                name: "Height",
                min: 0,
                max: 300,
                value: ""
            },
            [Parameter.color]: {
                type: "select",
                name: "Color",
                options: [
                    { label: "Black", value: "b" },
                    { label: "Red", value: "r" },
                    { label: "White", value: "w" },
                ],
                value: ""
            },
            [Parameter.size]: {
                type: "select",
                name: "Size",
                options: [
                    { label: "8px", value: "8" },
                    { label: "12px", value: "12" },
                    { label: "16px", value: "16" },
                    { label: "20px", value: "20" },
                    { label: "24px", value: "24" },
                ],
                value: ""
            },
            [Parameter.align]: {
                type: "select",
                name: "Align",
                options: [
                    { label: "Top Left", value: "tl" },
                    { label: "Top Center", value: "tc" },
                    { label: "Top Right", value: "tr" },
                    { label: "Center Left", value: "cl" },
                    { label: "Center Center", value: "cc" },
                    { label: "Center Right", value: "cr" },
                    { label: "Bottom Left", value: "bl" },
                    { label: "Bottom Center", value: "bc" },
                    { label: "Bottom Right", value: "br" },
                ],
                value: ""
            },
            [Parameter.name]: {
                type: "text",
                name: "Name",
                value: ""
            }
        };
        this.options = { ...this.options, ...options };
    }
    clone() {
        return new DisplayOptionValue();
    }
}
class DisplayOptionImage extends DisplayOption {
    constructor() {
        super();
        this.name = "Image";
        this.type = "image";
        let options = {
            [Parameter.x]: {
                type: "number",
                name: "X",
                min: 0,
                max: 400,
                value: ""
            },
            [Parameter.y]: {
                type: "number",
                name: "Y",
                min: 0,
                max: 300,
                value: ""
            },
            [Parameter.width]: {
                type: "number",
                name: "Width",
                min: 0,
                max: 400,
                value: ""
            },
            [Parameter.height]: {
                type: "number",
                name: "Height",
                min: 0,
                max: 300,
                value: ""
            },
            [Parameter.color]: {
                type: "select",
                name: "Color",
                options: [
                    { label: "Black", value: "b" },
                    { label: "Red", value: "r" },
                    { label: "White", value: "w" },
                ],
                value: ""
            },
            [Parameter.name]: {
                type: "text",
                name: "Name",
                value: ""
            },
        };
        this.options = { ...this.options, ...options };
    }

    clone() {
        return new DisplayOptionImage();
    }
}
class DisplayOptionPolygone extends DisplayOption {
    constructor() {
        super();
        this.name = "Polygone";
        this.type = "poly";
        let options = {
            [Parameter.size]: {
                type: "number",
                name: "Count",
                min: 1,
                max: 15,
                value: "",
                readOnly:true
            },
            [Parameter.name]: {
                type: "poly",
                name: "Points",
                max: 15,
                value: "",
                values: []
            },
            [Parameter.color]: {
                type: "select",
                name: "Color",
                options: [
                    { label: "Black", value: "b" },
                    { label: "Red", value: "r" },
                    { label: "White", value: "w" },
                ],
                value: ""
            },
        };
        this.options = { ...this.options, ...options };
    }
    clone() {
        return new DisplayOptionPolygone();
    }
}


class Display {
    constructor() {
        this.options = [
            new DisplayOptionPixel(),
            new DisplayOptionCircle(),
            new DisplayOptionCircleSegment(),
            new DisplayOptionLine(),
            new DisplayOptionRectangle(),
            new DisplayOptionPolygone(),
            new DisplayOptionText(),
            new DisplayOptionValue(),
            new DisplayOptionImage(),
        ];
        this.content = [

        ];
        this.text = "";

        this.outputTextarea = document.getElementById("outputTextarea");
        this.outputTextarea.value = `t»line¦x»20¦y»23¦x2»20¦y2»300¦c»b¦
t»line¦x»40¦y»23¦x2»40¦y2»300¦c»b¦
t»line¦x»60¦y»23¦x2»60¦y2»300¦c»b¦
t»line¦x»0¦y»23¦x2»60¦y2»23¦c»b¦
t»line¦x»0¦y»46¦x2»60¦y2»46¦c»b¦
t»line¦x»0¦y»69¦x2»60¦y2»69¦c»b¦
t»line¦x»0¦y»92¦x2»60¦y2»92¦c»b¦
t»line¦x»0¦y»115¦x2»60¦y2»115¦c»b¦
t»line¦x»0¦y»138¦x2»60¦y2»138¦c»b¦
t»line¦x»0¦y»162¦x2»60¦y2»162¦c»b¦
t»line¦x»0¦y»185¦x2»60¦y2»185¦c»b¦
t»line¦x»0¦y»208¦x2»60¦y2»208¦c»b¦
t»line¦x»0¦y»231¦x2»60¦y2»231¦c»b¦
t»line¦x»0¦y»254¦x2»60¦y2»254¦c»b¦
t»text¦x»0¦y»23¦w»20¦h»23¦c»b¦s»24¦a»cc¦l»A¦
t»text¦x»0¦y»46¦w»20¦h»23¦c»b¦s»24¦a»cc¦l»B¦
t»text¦x»0¦y»69¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»C¦
t»text¦x»0¦y»92¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»D¦
t»text¦x»0¦y»115¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»E¦
t»text¦x»0¦y»138¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»F¦
t»text¦x»0¦y»162¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»G¦
t»text¦x»0¦y»185¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»H¦
t»text¦x»0¦y»208¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»I¦
t»text¦x»0¦y»231¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»J¦
t»text¦x»0¦y»254¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»K¦
t»text¦x»0¦y»277¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»L¦
t»line¦x»0¦y»277¦x2»60¦y2»277¦c»b¦
t»text¦x»20¦y»23¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»M¦
t»text¦x»20¦y»46¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»N¦
t»text¦x»20¦y»69¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»O¦
t»text¦x»20¦y»92¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»P¦
t»text¦x»20¦y»115¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»Q¦
t»text¦x»20¦y»138¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»R¦
t»text¦x»20¦y»162¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»S¦
t»text¦x»20¦y»185¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»T¦
t»text¦x»20¦y»208¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»U¦
t»text¦x»20¦y»231¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»V¦
t»text¦x»20¦y»254¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»W¦
t»text¦x»20¦y»277¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»X¦
t»text¦x»40¦y»23¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»Y¦
t»text¦x»40¦y»46¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»Z¦
t»text¦x»40¦y»69¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»0¦
t»text¦x»40¦y»92¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»1¦
t»text¦x»40¦y»115¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»2¦
t»text¦x»40¦y»138¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»3¦
t»text¦x»40¦y»162¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»4¦
t»text¦x»40¦y»185¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»5¦
t»text¦x»40¦y»208¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»6¦
t»text¦x»40¦y»231¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»7¦
t»text¦x»40¦y»254¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»8¦
t»text¦x»40¦y»277¦w»20¦h»23¦c»b¦s»20¦a»cc¦l»9¦
t»rect¦x»0¦y»0¦w»20¦h»23¦c»b¦f»1¦
t»rect¦x»20¦y»0¦w»20¦h»23¦c»b¦f»1¦
t»rect¦x»40¦y»0¦w»20¦h»23¦c»b¦f»1¦
t»text¦x»0¦y»0¦w»20¦h»23¦c»w¦s»20¦a»cc¦l»1¦
t»text¦x»20¦y»0¦w»20¦h»23¦c»w¦s»20¦a»cc¦l»2¦
t»text¦x»40¦y»0¦w»20¦h»23¦c»w¦s»20¦a»cc¦l»3¦
t»line¦x»20¦y»0¦x2»20¦y2»23¦c»w¦
t»line¦x»40¦y»0¦x2»40¦y2»23¦c»w¦`;

        this.canvas = document.getElementById("displayCanvas");
        this.ctx = this.canvas.getContext("2d");

        this.drawId = this.ctx.createImageData(1, 1); // only do this once per page
        this.drawData = this.drawId.data;                        // only do this once per page
    }

    addContent(type) {
        let option = this.options.find((opt) => {
            return opt.type == type;
        });
        if (option) {
            this.content.push(option.clone());
            this.createContentHtml();
        }
        display.createOutput();
    }

    createOutput() {
        this.text = "";
        this.content.forEach((element) => {
            this.text += element.getOutput() + "\n";
        });
        this.outputTextarea.value = this.text;
    }

    loadFromTextBox() {
        let text = this.outputTextarea.value;
        
        let textOption = {};

        let key = true;
        let keytext = "";
        let value = false;
        let valuetext = "";
        let content = [];
        //t»line¦x»20¦y»23¦x2»20¦y2»300¦c»b¦
        for (let a=0; a < text.length;a++) {
            let char =text.charAt(a);
            let charcode = text.charCodeAt(a);
            if (key) {
                if (charcode == 187) {
                    key = false;
                    value = true;
                }
                else if (char == '\n') {
                    content.push(textOption);
                    textOption = {};
                }
                else {
                    keytext += char;
                }
            }
            else if (value) {
                if (charcode == 166) {
                    key = true;
                    value = false;
                    textOption[keytext] = valuetext;
                    valuetext = "";
                    keytext  = "";
                }
                else {
                    valuetext += char;
                }
            }
        }

        content = content.map((element) => {

            if (element.t == null || element.t == undefined) {
                return null;
            }

            let option = this.options.find((opt) => {
                return opt.type == element.t;
            });

            if (!option) {
                return null;
            }
            let instance = option.clone();
            let keys = Object.keys(instance.options);
            keys.forEach(key => {
                if (element[key] != null && element[key] != undefined) {
                    console.log("set Option", key)
                    instance.options[key].value = element[key];
                }
            });
            console.log(element, instance.options);
            return instance;
        }).filter((element) => {

            return element != null;
        });

        this.content = this.content.concat(content);
        this.createContentHtml();
        this.createOutput();
    }

    createContentHtml() {
        let OptionsDiv = document.getElementById("OptionsDiv");
        OptionsDiv.innerHTML = "";

        let table = document.createElement("table");
        table.classList.add("contentTable");
        // Header
        let headernames = [];
        headernames.push("Controls");
        headernames = headernames.concat(Object.keys(Parameter));
        headernames = headernames.map((name) => {
            return name.charAt(0).toUpperCase() + name.slice(1);
        });
        let header = document.createElement("tr");
        table.appendChild(header);
        headernames.forEach(name => {
            let row = document.createElement("th");
            row.innerHTML = name;
            header.appendChild(row);
        });
        OptionsDiv.appendChild(table);



        display.content.forEach((option, index) => {
            if (!option.hide) {
                option.createContentHtml(table);
            }

        });
    }

    createOptionsHtml() {
        let OptionsDiv = document.getElementById("OptionsAddDiv");
        display.options.forEach(option => {

            option.createOptionsHtml(OptionsDiv);


        });
    }

    setDisplay() {
        console.log("text", this.text,this.text.length);
        
        return postData("/api/display/image", this.text);

        return new Promise((resolve) => { resolve() });
    }
    updateDisplay() {
        return new Promise((resolve) => {


            this.drawData[0] = 0; // R
            this.drawData[1] = 255; // G
            this.drawData[2] = 0; // B
            this.drawData[3] = 100; // A
            this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
            setTimeout(() => {
                getData("/api/display/image", (reader) => {
                    var total = 0
                    var x = 0;
                    var y = 0;
                    var width = 400;
                    var height = 300;
                    var bytesize = 8;
                    display.drawData[0] = 0; // R
                    display.drawData[1] = 0; // G
                    display.drawData[2] = 0; // B
                    display.drawData[3] = 255; // A
                    return pump()
                    function pump() {
                        return reader.read().then(({ done, value }) => {
                            if (done) {

                                return;
                            }
                            for (let a = 0; a < value.byteLength; a++) {
                                let byte = value[a];
                                if (total == width * height / bytesize) {
                                    x = 0;
                                    y = 0;
                                    display.drawData[0] = 255; // R
                                    display.drawData[1] = 0; // G
                                    display.drawData[2] = 0; // B
                                    display.drawData[3] = 255; // A
                                    console.log("Switch to red");
                                }
                                for (let b = (bytesize - 1); b >= 0; b--) {

                                    if ((byte & 1 << b) != 0) {
                                        display.ctx.putImageData(display.drawId, x, y);
                                    }
                                    x++;
                                    if (x >= 400) {
                                        x = 0;
                                        y++;
                                    }

                                }
                                total++;
                            }
                            console.log(`received ${value.byteLength} bytes (${total} bytes in total)`)


                            return pump()
                        })
                    }
                }).then(() => {
                    resolve();
                }, (err) => {
                    resolve();
                });
            })




            /*
             getData("/api/display/image").then((text) => {
     
                 //let text = testData;
                 var width = 400;
                 var height = 300;
                 var bytesize = 8;
     
                 
                 let len1 = width * height / bytesize;
     
                 let x = 0;
                 let y = 0;
     
     
                 for (let a = 0; a < len1; a++) {
                     let byte = text.charCodeAt(a);
     
                     for (let b = (bytesize - 1); b >= 0; b--) {
                         console.log("tt", x, y, b, a, byte, (byte & 1 << b) != 0);
                         if ((byte & 1 << b) != 0) {
                             this.ctx.putImageData(this.drawId, x, y);
     
                         }
                         x++;
                         if (x >= 400) {
                             x = 0;
                             y++;
                         }
                         if (x == width && y == height) {
                             break;
                         }
                     }
                 }
                 x = 0;
                 y = 0;
     
                 this.drawData[0] = 255; // R
                 this.drawData[1] = 0; // G
                 this.drawData[2] = 0; // B
                 this.drawData[3] = 1; // A
                 //ctx.putImageData(this.drawId, x, y);
             });
             //*/
        })
    }


}

var display = new Display();
function UploadDisplayData() {
    display.setDisplay().then(() => {
        setTimeout(() => {
            display.updateDisplay();
        }, 2500);
    });
}
function UpdateDisplay() {

    setTimeout(() => {
        display.updateDisplay();
    }, 10);
}
function Unhide() {
    display.content.forEach((element) => {
        element.hide = false;
    });
    display.createContentHtml();
}

function LoadTextClear() {
    display.content = [];
    display.loadFromTextBox();
}

function LoadTextAppend() {
    display.loadFromTextBox();
}
window.onload = (() => {

    baseurl = "http://192.168.0.94";
    console.log(display);

    display.createOptionsHtml();
    display.createContentHtml();
    // display.updateDisplay();

    let urlInput = document.getElementById("UrlInput");
    if (urlInput.value.length == 0) {
        urlInput.value = baseurl;
    }
    else {
        baseurl = urlInput.value;
    }

    urlInput.onchange = ((ev) => {
        baseurl = ev.target.value

        console.log(baseurl);

        getData("").then((text) => {
            console.log("GET", text);
        });

        
    });

});
