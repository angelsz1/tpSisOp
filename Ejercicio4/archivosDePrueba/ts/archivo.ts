import { animate, style, transition, trigger } from '@angular/animations';
import { Component, OnInit } from '@angular/core';

/*
  COMENTARIO MULTILINEA
*/

@Component({
  selector: 'app-top-nav',
  templateUrl: './top-nav.component.html',
  styleUrls: ['./top-nav.component.scss'],
  animations: [
    trigger('openCloseAnimation', [
      transition(':enter', [
        style({ height: 0, overflow: 'hidden' }),
        animate('0.3s', style({ height: '!' }))
      ]),
      transition(':leave', [
        style({ height: '!', overflow: 'hidden' }),
        animate('0.3s', style({ height: '0' }))
      ])
    ])
  ]
})
//Ejemplo de codigo Typescript
export class HeaderComponent implements OnInit {

  public hideNav: boolean = false;

  constructor() { }

  ngOnInit(): void {
    if (window.matchMedia("(max-width: 767px)").matches)
    {
      this.hideNav = true;
    }
  }


  /*NO ME INTERESA
  toggleNav() {
    if(this.hideNav == true) {
      this.hideNav = false;
    }
    else {
      this.hideNav = true;
    }
  }*/
}
