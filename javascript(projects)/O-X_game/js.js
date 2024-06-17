const publish_board = document.querySelector(".publish_board");
const boxes = document.querySelectorAll(".box");
const new_game = document.querySelector(".new_game");
const win = document.querySelector(".win");

let current_player = "";
let arr;
let count;
const winnerList = [ 
[0, 1, 2], 
[3, 4, 5], 
[6, 7, 8],
[0, 3, 6],
[1, 4, 7], 
[2, 5, 8], 
[0, 4, 8],
[2, 4, 6]];

function fresh_game() {
  current_player = "X";
  count = 0;
  publish_board.innerText = "current player - " + current_player;
  arr = ["", "", "", "", "", "", "", "", ""];
  boxes.forEach((box, index) => {
    box.innerText = "";
    box.style.pointerEvents = "all";
    box.classList.remove("win");
  });
  new_game.classList.remove("active");
};
fresh_game();



boxes.forEach((box, index) => {
  box.addEventListener("click", () => {
    manager(index);
    Switch();
  });
});



function Switch() {
  if (current_player == "X") { current_player = "O"; }
  else { current_player = "X" }
  publish_board.innerText = "current player - " + current_player;
};


function manager(index) {
  if (arr[index] == "") {
    arr[index] = current_player;
    boxes[index].innerText = current_player;
    count++;
    boxes[index].style.pointerEvents = "none";
    if (count > 4) { check_win(); }
  }
};

function check_win() {
  let winner = "";
  winnerList.forEach((position) => {
    if (arr[position[0]] != "" && arr[position[1]] != "" && arr[position[2]] != "" && arr[position[0]] === arr[position[1]] && arr[position[1]] === arr[position[2]]) {
      if (arr[position[0]] == "X") { winner = "X"; } else { winner = "O"; }
      boxes.forEach((box) => {
        box.style.pointerEvents = "none";
    });
    console.log(position[0]);
      boxes[position[0]].classList.add("win");
      boxes[position[1]].classList.add("win");
      boxes[position[2]].classList.add("win");
      
    }
  });
  if (winner != "") {
    publish_board.innerText = "winner - " + winner;
    new_game.classList.add("active");
  }

};
new_game.addEventListener("click", () => {
  fresh_game();
  console.log("freash_game");
});