const popp = document.querySelector(".popupmodal");
const blurr=document.querySelector(".backblur");

// console.log(popp);


const openpopup = () => {
   popp.classList.add("active");
    blurr.classList.add("bluractive")
};


const closepopup = () => {
    popp.classList.remove("active");
     blurr.classList.remove("bluractive")
 };