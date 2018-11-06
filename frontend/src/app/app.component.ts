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
  search_text: string = "book";
  docs: Doc[] = [];

  ngOnInit(): void {
    this.docs.push({
      url: "123",
      score: 12,
      snippet: "24"
    });
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
