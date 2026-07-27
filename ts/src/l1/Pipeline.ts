// Copyright (c) 2026 Scott Tringali. All rights reserved.

import { Transform } from './Transform';
import { Parameters } from './Parameters';

export class Pipeline<T, P extends Parameters = Parameters> implements Transform<T, P> {
    private entries: Pipeline.Strategy<T, P>[] = [];

    install(priority: number, name: string, transform: Transform<T, P>): void {
        if (this.entries.some(e => e.priority === priority)) {
            throw new Error(`Duplicate priority ${priority}`);
        }
        this.entries.push({ priority, name, transform });
        this.entries.sort((a, b) => a.priority - b.priority);
    }

    installNew<Args extends unknown[]>(priority: number, name: string, ctor: new (...args: Args) => Transform<T, P>, ...args: Args): void {
        this.install(priority, name, new ctor(...args));
    }

    apply(value: T, params: P): T {
        let result = value;
        for (const { transform } of this.entries) {
            const { accepts, rejects, handles } = transform;
            if (handles && !handles(params)) {
                continue;
            }
            if (rejects && rejects(result)) {
                continue;
            }
            if (accepts && !accepts(result)) {
                continue;
            }
            result = transform.apply(result, params);
        }
        return result;
    }
}

namespace Pipeline {
    export interface Strategy<T, P extends Parameters> {
        priority: number;
        name: string;
        transform: Transform<T, P>;
    }
}
