//
// Created by archie on 4/1/17.
//

#include "Server.h"
#include "Hash.h"

Server::Server() {
    clientPipeName = "fromClient"; //pomenovana rura
    serverPipeName = "fromServer";  //pomenovana rura2
}

void Server::run() {
    communicator.createIO(serverPipeName, clientPipeName, false); //Nadviazanie spojenia medzi klientom a serverom, treba najskor vytvorit 1 pipe s readerom a writerom a potom druhu, preto klient najskor jej reader a potom citatel
    communicator.createTunnel();    //Vytvorenie tunelu medzi serverom a klientom
    verifyClient();         //Overenie klienta
    listen();   //Cakame na spravy od klienta
}

void Server::verifyClient() {
    setupFFSParameters();
    int correctAnswers = 0;

    for(int i = 0; i < tSecurityParameter; i++) {
        readX();
        sendRandomBitsVector();
        readResponse();
        if(isResponseOk())
            correctAnswers++;
    }

    if(correctAnswers == tSecurityParameter)
        cout << "CLIENT VERIFIED" << endl;
    else
        cout << "ERROR: CLIENT NOT VERIFIED" << endl;
}

void Server::setupFFSParameters() {
    mpz_class commonModulus("71183552285038461766113647959396358732706491637429430926545437207184125238309080283521732845130075390845656628117522464822858239853139020661108679805853177746229905002767516006612801512117052998585896447280921555208648711629997437345455618254036492687238619118038417360819908894719307131044594675256506148315721871893790592922471352116368798813460668095376709055498641058388154371624108496453083237469650211111431015970756158890041945366886657631236160844900022246123320938053062120230536865298378252895800036092900147260312150394885899649114326431821546855506303256064596753532612324481383960608160126983904724442161", base);
    vector<mpz_class> publicVector(kSecurityParameter);
    publicVector[0].set_str("24495216945444909242638268555190461072241086584979286679588097290403953333437883286032576737272225990848868402929469443199202643513834232073488140254215734984652034004424797947082373126847003220648156272021953187826209994808074294324375575356701020055828800825220468297812773124568851550988167301525005764499833897436941403655158069687868499503791956398812540207539307643998364316554023921849376927528927708915426942136470631999408769556116653815276013773855217233251846973663817284501707856703025362087818453796326963277263587255733409476602210773132182233100017748949023788150292095697488677230984459220262486961167", base);
    publicVector[1].set_str("61561395245396833742931814473712656177337646303521051669598122748415835766733810134295633976257269242586183526581293035257235699637957322757307499101241355008253364195641208084171861156551709390745792361690738152723026759902464499687930497241377019160777259206438001376351664199055407247914658961858968399917725666496611437501892147324459314809454514457713028974160827465952076978897146768393059240376848874587998963310681225473728815436601541385839070358487551974157417229741241412969412038526110144349302923844329093702882633909244418143969545583290755103310754199560478455430917324446248393821122357997884017128907", base);
    publicVector[2].set_str("29567152708801862366343051141254835792696629020222228694858184973733314527095003568393322707027095634324024555671865777487360492237947383234958896398166467184254009937040455785943388820372349338098953382072761604615652523884301765483961202275876665188575784147220053079880397117553369664974673070943812494521742613698890796629606770862311129667512916378922428764253901317578119094664144400106402316273803264498132040356675025997661791007574694517611766296860347607469636763491611535179135715479428438236554293411225858195555206376337629076904873724866067194047694261240924067933403126722349715709692292545623632422755", base);
    publicVector[3].set_str("37442372729620692580069639686405790165809198526867236355760073642807252668643590833924455519899886252824910150370222111842863684959593206275662981936094684027313031112584788017790954768568713065818251007136183285500634935690399998027217819316696137994248794903508562434323691351822330510041319850507600636194357453335419952204210370861753217346194819203922926257707647462762222086050770607695131433708573574167070861834481209355239931335770462000611842377339053562381608327255372274220661526195644819134620486965827085082589219192814614244728880634698163619637943283478174415213185916830949011290214328290444785449476", base);
    publicVector[4].set_str("8281886081303335572521956147570153316727159037903183138532289714246352382797083162319587814109363552350245150857817763566475610997954049165240253229212729752135242386022741019221131171134823690515366525375608825151609371123051801433090279941387765620264924160526437692540682088767340182328825147958027388860033131862000998898144372592506265175016397144062093097045943914757315030113819259906136492915187369605307792994047153959651215984492904642740770298412146094705051812062028925222512513525380249935078877934722774258511118185757627463424976262733424936303157658353125212630795127716310323834322656703658398723240", base);

    ffs.setNCommonModulus(commonModulus);
    ffs.setPublicVector(publicVector);
}

void Server::readX() {
    ffs.setXForB(mpz_class(communicator.readEncryptedMsg(), base));
}

void Server::sendRandomBitsVector() {
    ffs.setRandomBitsVector(ffs.generateRandomBitsVector());
    communicator.sendEncryptedMsg(ffs.getRandomBitsVectorStr());
}

void Server::readResponse() {
    ffs.setResponse(mpz_class(communicator.readEncryptedMsg(), base));
}

bool Server::isResponseOk() {
    return ffs.verifyResponse(ffs.getResponse());
}

void Server::listen() {
    while(1) {
        string message(communicator.readEncryptedMsg());
        string hash(Hash::createSHA256(message));
        communicator.sendEncryptedMsg(hash);
        if(message == "exit")
            break;
    }
}


