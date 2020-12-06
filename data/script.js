$(document).ready(function(){
    $("#appliquerNom").click(function(){
        var nom = $("#inputNomAquarium").val();
        $.post("inputNomAquarium",{
            proprieteNom: nom
        });
    });
});

$(document).ready(function(){
    $("#appliquerTemp").click(function(){
        var tempMin = $("#inputTempMin").val();
        var tempMax = $("#inputTempMax").val();
        $.post("inputTempAquarium", {
            proprieteTempMin: tempMin,
            proprieteTempMax: tempMax
        });
    });
});

$(document).ready(function(){
    $("#appliquerPompe").click(function(){
        var pompeDuree = $("#inputPompeDuree").val();
        var pompeTimer = $("#inputPompeTimer").val();
        $.post("inputPompeAquarium", {
            proprietePompeDuree: pompeDuree,
            proprietePompeTimer: pompeTimer
        });
    });
});