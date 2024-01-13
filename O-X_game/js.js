const publish_board=document.querySelector(".publish_board");
const boxes=document.querySelector(".boxes");
const box=document.querySelector(".box");
const new_game=document.querySelector(".new_game");
const win=document.querySelector(".win");

let current_player="";
let arr=["","","","","","","","",""];
let count=0;
function fresh_game(){
  current_player="X";
  count=0;
  publish_board.innerText="current player"+{current_player};
  arr=["","","","","","","","",""];
    boxes.forEach(element,index => {
        boxes[index].classList.remove("win");
        boxes[index].innerText="";
    });
    new_game.classList.remove("active");

}
fresh_game();
boxes.forEach(element,index => {
box.addEventListener("click",()=>{
  manager(index);
  Switch(current_player);
})
})


function Switch(current_player){
    if(current_player=="X"){current_player="O";}
    else{current_player=="X"}
    publish_board.innerText="current player"+{current_player};  
}


function manager(index){
    if(arr[index]==""){
arr[index]=current_player;
boxes[index].innerText=current_player;
count++;
if(count>5)check_win();
}}

function check_win(){

}
