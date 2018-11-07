import { Injectable } from '@angular/core';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import {Observable} from "rxjs";
import {Doc} from "./Doc";

@Injectable()
export class SearchService {

  constructor(
    private http: HttpClient
  ) { }

  // get query result from server
  getSearchResult(query: string): Observable<Doc[]> {

    return this.http.get<Doc[]>(`http://localhost:3000/search/?text=${query}`);
  }

}
