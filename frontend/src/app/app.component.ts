import {Component, OnInit} from '@angular/core';
import {Doc} from "./Doc";
import {SearchService} from "./search.service";

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent implements OnInit {
  title = 'my search engine';
  search_text: string = "";
  docs: Doc[] = [];

  ngOnInit(): void {
  }

  constructor(
    private searchService: SearchService
    ) { }

  search(): void {
    this.searchService.getSearchResult(this.search_text).
    subscribe(docs => {

      this.docs = docs;
      console.log(this.docs);

    });
  }

}
