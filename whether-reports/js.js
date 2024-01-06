const API_key='d1845658f92b31c64bd94f06f7188c9c';

const city=document.querySelector(".city");
const Lat=document.querySelector(".lat");
const Long=document.querySelector(".long");
my_location()

console.log(city.innerText);

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


function my_location(){
    if(navigator.geolocation){
        navigator.geolocation.getCurrentPosition(showposition);
    }
    else{
        console.log("failed to get target location");        
    }

}

function showposition(position){
 
    Lat.innerText = position.coords.latitude;
    Long.innerText = position.coords.longitude;
    meramosam();
}

async function meramosam(){
    try{
    const api_la = await fetch(`https://api.openweathermap.org/data/2.5/weather?lat=${Lat.innerText}&lon=${Long.innerText}&appid=${API_key}`);
    const data = api_la.json();
    console.log("data",data);
    }
    catch(e){
        console.log("unable to fetch data",e);
    }
} 
