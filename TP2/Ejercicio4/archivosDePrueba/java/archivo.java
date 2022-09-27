/* Este es un codigo de prueba para testear el script*/                
package sistOp.controladores;

import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

//HEALT CHECK CONTROLLER
@RestController
public class HealtCheckController {

    @GetMapping(path = "/ping")
    public String pong() {
        return "pong"; //Returns pong
    }

    /* DEPRECATED ENDPOINT
    @GetMapping(path = "/ping")
    public String pong() {
        //Returns pong
        return "pong";
    }
    */           
}