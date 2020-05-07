function onArrosage()
{
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "on", true);
    xhttp.send();
    if (document.getElementById("Arrosage").innerText == "Arroser")
    {
        document.getElementById("Arrosage").innerHTML = "Stopper l'arrosage";
        document.getElementById("ArrosageBtn").style.background = 'crimson';
        document.getElementById("ArrosageBtn").style.color = 'white';
    }
    else
    {
        document.getElementById("Arrosage").innerHTML = "Arroser";
        document.getElementById("ArrosageBtn").style.background = 'cornflowerblue';
        document.getElementById("ArrosageBtn").style.color = 'white';
    }
}

function onEclairage()
{
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "on", true);
    xhttp.send();
    if (document.getElementById("Eclairage").innerText == "Eclairer")
    {
        document.getElementById("Eclairage").innerHTML = "Stopper l'éclairage";
        document.getElementById("EclairageBtn").style.background = 'crimson';
        document.getElementById("EclairageBtn").style.color = 'white';
    }
    else
    {
        document.getElementById("Eclairage").innerHTML = "Eclairer";
        document.getElementById("EclairageBtn").style.background = 'gold';
        document.getElementById("EclairageBtn").style.color = 'white';
    }
}

setInterval(function getData()
{
    var xhttp = new XMLHttpRequest();

    xhttp.onreadystatechange = function()
    {
        if (this.readyState == 4 && this.status == 200)
        {
            document.getElementById("valeurTemperatureAir").innertHTML = this.reponseText;
            document.getElementById("TemperatureAir").style.strokeDashoffset = 320 - (320 * this.response)/50;
        }
    };

    xhttp.open("GET","lireTemperatureAir", true);
    xhttp.send();
}, 1000);


function readTextFile(file)
{
    var tableau = [];
    var rawFile = new XMLHttpRequest();
    rawFile.open("GET", file, false);
    rawFile.onreadystatechange=function()
    {
        if (rawFile.readyState === 4)
        {
            if (rawFile.status === 200 || rawFile.status == 0)
            {
                var text = rawFile.responseText;
                if (text != null)
                {
                    (text.split(";")).forEach(function(element)
                    {
                        var tableauInt = [];
                        (element.split(",")).forEach(function(i)
                        {
                            var reel = parseFloat(i);
                            tableauInt.push(reel);
                        });
                        tableauInt[0] = tableauInt[0]*1000;
                        tableau.push(tableauInt);
                    });
                }
                tableau.pop();
                console.log(tableau[-1]);
                createChart(tableau);
            }
        }
    }
    rawFile.send(null);
}