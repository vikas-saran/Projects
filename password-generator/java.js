const passout = document.querySelector("#passout");
const copyimg = document.querySelector("#copyimg");
const Copiedbox = document.querySelector("#copiedbox");
const passlenvalue = document.querySelector("#passlen-value");
const slidecontainer = document.querySelector(".slidecontainer");
const uppercase = document.querySelector("#uppercase");
const lowercase = document.querySelector("#lowercase");
const addnumber = document.querySelector("#addnumber");
const addsymbol = document.querySelector("#addsymbol");
const allCheckbox = document.querySelectorAll("input[type=checkbox]");
const strengthcolor = document.querySelector("#strength-color");
const generatebtn = document.querySelector(".generatepass");


const symbols = '~`!@#$%^&*()_-+={[}]|:;"<,>.?/';


let password = "";
let passwordlength = 10;
let checkCount = 1;

uppercase.checked = true;

circlecolor('#FFFFFF');

function rangefind(min,max){
    return Math.floor(Math.random()*(max-min))+min;
}

function onslidehandel(){
    slidecontainer.value=passwordlength;
    passlenvalue.innerText=passwordlength;
}
onslidehandel()

function strengthcalculate(){
    let arr =["#F0D8FF","#99FF33","#FFFF00","#FFA500","#ff3300","#8B0000"];
    let p=Math.floor((passwordlength/10)*(checkCount/1.6));
    circlecolor(arr[p]);
}

function circlecolor(color){
    strengthcolor.style.backgroundColor = color;
    strengthcolor.style.boxShadow = `0px 0px 12px 1px ${color}`;
    console.log(color);
}
 function generaterandomnum(){
    return rangefind(0,9);
 }

function generaterandomlower(){
     return String.fromCharCode(rangefind(97,123));
}

function generaterandomupper(){
    return String.fromCharCode(rangefind(65,91));
}

function generaterandomsymbol(){
    return symbols.charAt(rangefind(0,symbols.length));
}
function suffelpass(array){
    for (let i = array.length - 1; i > 0; i--) {
        // find out random j
        const j = Math.floor(Math.random() * (i + 1));
        // swap 2 numbers
        const temp = array[i];
        array[i] = array[j];
        array[j] = temp;
      }
      let str = "";
      // array.forEach((el) => (str += el));
      str = array.join("");
      return str;
}

async  function copyonboard() {
    try {
      await navigator.clipboard.writeText(passout.value);
      Copiedbox.innerText="Copied";
      console.log('Content copied to clipboard');
    } catch (err) {
        Copiedbox.innerText="faliure";
      console.error('Failed to copy: ', err);
    }
    Copiedbox.classList.add('active');
    setTimeout(() => {
        Copiedbox.classList.remove('active');
    }, 2000);
}

slidecontainer.addEventListener('input',(e) => {
    passwordlength=e.target.value;
    onslidehandel();
})

allCheckbox.forEach((checkbox)=>{
    checkbox.addEventListener('change',()=>{
        checkCount=0;
        allCheckbox.forEach((checkbox)=>{
            if(checkbox.checked){
            checkCount++;}
        })

        if(checkCount>passwordlength){
            passwordlength=checkCount;
            onslidehandel();
        }

    })
})

copyimg.addEventListener('click',()=>{
    if(passout.value){
        copyonboard();
    }
})

generatebtn.addEventListener('click', ()=>{
    if(checkCount==0){return;}
    if(checkCount>passwordlength){
        passwordlength=checkCount;
        onslidehandel();
    }
    password="";
    let genarr=[];
    if(uppercase.checked){
        password +=generaterandomupper();
        genarr.push(generaterandomupper);
    }
    if(lowercase.checked){
        password +=generaterandomlower();
        genarr.push(generaterandomlower);
    }
    if(addnumber.checked){
        password +=generaterandomnum();
        genarr.push(generaterandomnum);
    }
    if(addsymbol.checked){
        password +=generaterandomsymbol();
        genarr.push(generaterandomsymbol);
    }
    
    for(let i=0;i<passwordlength-checkCount;i++){
          password+=genarr[rangefind(0,checkCount-1)]();
    }
   password=suffelpass(Array.from(password));
   passout.value=password
   strengthcalculate();
    
   
})
