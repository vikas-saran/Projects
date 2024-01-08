const search_wheather_tab=document.querySelector("#search_wheather_tab");
const my_wheather_tab=document.querySelector("#my_wheather_tab");
const grant_access=document.querySelector(".grant_access");
const search_bar=document.querySelector(".search_bar");
const loding_screen=document.querySelector(".loding_screen");
const whetherdata=document.querySelector(".whetherdata");

const API_key='d1845658f92b31c64bd94f06f7188c9c';
const current_tab=my_wheather_tab;
current_tab.classList.add("curren_tab");


function switch_screen(clicked_tab){

   if(current_tab!=clicked_tab){
        current_tab.classList.remove("current_tab");
        current_tab=clicked_tab
        current_tab.classList.add("current_tab");
        if(!search_bar.classList.contains("active")){
            whetherdata.classList.remove("avtive");
            grant_access,classList.remova("active");
            search_bar.classList.add("active");
        }
        else{
            search_bar.classList.remove("active");
            whetherdata.classList.remove("avtive");
            check_access();
        }
   } 
}

function check_access(){
    const local_cordinates=cordinate_storage.getItem('cordinate_data');
    if(!local_cordinates){
        grant_access.classList.add("active");
    }
    else{
        const cordinate=JSON.parse.apply(local_cordinates);
        meramosam(cordinate);
    }

}

function my_location(){
    if(navigator.geolocation){
        navigator.geolocation.getCurrentPosition(showposition);
    }
    else{
        console.log("failed to get target location");        
    }

}

function showposition(position){
    const cordinate_data = position.coords.latitude+position.coords.longitude;

}

async function meramosam(cordinate){
    const {lat ,lon}= cordinate;
    grant_access.classList.remove("active");
    loding_screen.classList.add("active");
    try{
    const api_la = await fetch(`https://api.openweathermap.org/data/2.5/weather?lat=${Lat.innerText}&lon=${Lon.innerText}&appid=${API_key}`);
    const data = api_la.json();
    loding_screen.classList.remove("active");
    whetherdata.classList.add("active");
    publish_data(data);

    }
    catch(e){
        console.log("unable to fetch data",e);
    }
}

function publish_data(data){
    
}

async function mosam(city){
    try{
    const api_la = await fetch(`https://api.openweathermap.org/data/2.5/weather?q=${city.innerText}&appid=${API_key}`);
    const data = api_la.json();
    console.log("data",data);
    }
    catch(e){
        console.log("unable to fetch data",e);
    }
} 


 



search_wheather_tab.addEventListener("click",()=>{
    switch_screen(search_wheather_tab);
})

my_wheather_tab.addEventListener("click",()=>{
    switch_screen(my_wheather_tab);
})